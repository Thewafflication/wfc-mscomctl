// The behaviour of the controls of the library when a COM client creates them
// (see wfc/ocx/class_factory.hpp). The rules themselves are the library's
// models, which the WFC evaluator uses too. Windows only; internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL_COM_BEHAVIOUR_HPP
#define WFC_MSCOMCTL_COM_BEHAVIOUR_HPP

#include "com_collections.hpp"

#include <wfc/mscomctl/range.hpp>
#include <wfc/ocx/class_factory.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace wfc::mscomctl {

// A ProgressBar (Single properties) or a Slider (Long properties) keeps Min,
// Max and Value in its range model: the range stays ordered, Value is kept
// inside it and a value outside it is error 380. A change of the Value of a
// Slider raises Change. The other properties are stored by the control as for
// any member.
class RangeBehaviour final : public wfc::com::Behaviour {
public:
    RangeBehaviour(const double minimum, const double maximum,
                   const bool slider)
        : model_(minimum, maximum, 0.0), slider_(slider) {}

    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>* events) override {
        if (member != "min" && member != "max" && member != "value") {
            return S_FALSE;
        }
        VARIANT number;
        VariantInit(&number);
        if (VariantChangeType(&number, const_cast<VARIANT*>(&value), 0,
                              VT_R8) != S_OK) {
            *error = 13;  // type mismatch
            return E_FAIL;
        }
        // A Slider holds whole numbers.
        const double wanted =
            slider_ ? static_cast<double>(static_cast<long>(number.dblVal))
                    : number.dblVal;
        const auto outcome = member == "min"   ? model_.set_min(wanted)
                             : member == "max" ? model_.set_max(wanted)
                                               : model_.set_value(wanted);
        if (!outcome.valid) {
            *error = 380;  // invalid property value
            return E_FAIL;
        }
        if (slider_ && outcome.changed) {
            events->push_back("Change");
        }
        return S_OK;
    }

    HRESULT get(const std::string& member, const Index&, VARIANT* result,
                long*) override {
        double number = 0.0;
        if (member == "min") {
            number = model_.min();
        } else if (member == "max") {
            number = model_.max();
        } else if (member == "value") {
            number = model_.value();
        } else {
            return S_FALSE;
        }
        if (slider_) {
            result->vt = VT_I4;
            result->lVal = static_cast<long>(number);
        } else {
            result->vt = VT_R4;
            result->fltVal = static_cast<float>(number);
        }
        return S_OK;
    }

private:
    RangeModel model_;
    bool slider_;
};

// A control with one collection (TabStrip.Tabs, StatusBar.Panels,
// Toolbar.Buttons): the property of that name answers the collection, which is
// late bound (com_collections.hpp), and a TabStrip also keeps SelectedItem.
class CollectionBehaviour final : public wfc::com::Behaviour {
public:
    CollectionBehaviour(const char* property, const com::KindSpec& kind)
        : property_(property),
          state_(std::make_shared<com::ListState>(kind)),
          collection_(new com::CollectionObject(state_)) {}

    ~CollectionBehaviour() override { collection_->Release(); }

    HRESULT get(const std::string& member, const Index& index, VARIANT* result,
                long* error) override {
        if (member == property_) {
            if (index.count == 1U) {
                // `Tabs(1)` is `Tabs.Item(1)`.
                EXCEPINFO exception{};
                const HRESULT outcome =
                    collection_->item(index.arguments[0], result, &exception);
                if (outcome == DISP_E_EXCEPTION) {
                    *error = exception.scode & 0xFFFF;
                    SysFreeString(exception.bstrSource);
                    SysFreeString(exception.bstrDescription);
                    return E_FAIL;
                }
                return outcome;
            }
            result->vt = VT_DISPATCH;
            result->pdispVal = collection_;
            collection_->AddRef();
            return S_OK;
        }
        if (member == "selecteditem" && state_->spec->selectable) {
            if (state_->selected == nullptr) {
                com::detail::set_nothing(result);
                return S_OK;
            }
            result->vt = VT_DISPATCH;
            result->pdispVal = new com::ItemObject(state_, state_->selected);
            return S_OK;
        }
        return S_FALSE;
    }

    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>*) override {
        if (member != "selecteditem" || !state_->spec->selectable) {
            return S_FALSE;
        }
        VARIANT plain;
        VariantInit(&plain);
        if (VariantCopyInd(&plain, &value) != S_OK) {
            *error = 13;
            return E_FAIL;
        }
        HRESULT outcome = S_OK;
        if (plain.vt == VT_DISPATCH && plain.pdispVal == nullptr) {
            state_->selected.reset();  // Nothing
        } else {
            com::ItemObject* item = nullptr;
            if (plain.vt == VT_DISPATCH &&
                plain.pdispVal->QueryInterface(
                    com::detail::kItemIid, reinterpret_cast<void**>(&item)) ==
                    S_OK) {
                const auto& items = state_->items;
                if (item->state() == state_ &&
                    std::find(items.begin(), items.end(), item->data()) !=
                        items.end()) {
                    state_->selected = item->data();
                } else {
                    *error = 13;  // not an item of this control
                    outcome = E_FAIL;
                }
                item->Release();
            } else {
                *error = 13;
                outcome = E_FAIL;
            }
        }
        VariantClear(&plain);
        return outcome;
    }

private:
    std::string property_;
    std::shared_ptr<com::ListState> state_;
    com::CollectionObject* collection_;
};

// The behaviour of the class called `class_name`, none for most of them yet.
inline std::unique_ptr<wfc::com::Behaviour> make_behaviour(
    const char* class_name) {
    if (_stricmp(class_name, "TabStrip") == 0) {
        return std::make_unique<CollectionBehaviour>("tabs", com::tab_spec());
    }
    if (_stricmp(class_name, "StatusBar") == 0) {
        return std::make_unique<CollectionBehaviour>("panels",
                                                     com::panel_spec());
    }
    if (_stricmp(class_name, "Toolbar") == 0) {
        return std::make_unique<CollectionBehaviour>("buttons",
                                                     com::button_spec());
    }
    if (_stricmp(class_name, "ProgressBar") == 0) {
        return std::make_unique<RangeBehaviour>(0.0, 100.0, false);
    }
    if (_stricmp(class_name, "Slider") == 0) {
        return std::make_unique<RangeBehaviour>(0.0, 10.0, true);
    }
    return nullptr;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_COM_BEHAVIOUR_HPP
