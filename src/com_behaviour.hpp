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

// A control with collections (TabStrip.Tabs, StatusBar.Panels,
// Toolbar.Buttons, ListView.ListItems and ColumnHeaders, ImageList.ListImages,
// ImageCombo.ComboItems): the property of that name answers the collection,
// which is late bound (com_collections.hpp), and a control whose items can be
// selected (a TabStrip, an ImageCombo) also keeps SelectedItem.
class CollectionBehaviour final : public wfc::com::Behaviour {
public:
    struct Entry {
        const char* property;  // lower case
        const com::KindSpec* kind;
    };

    explicit CollectionBehaviour(const std::vector<Entry>& entries) {
        for (const Entry& entry : entries) {
            Held held;
            held.property = entry.property;
            held.state = std::make_shared<com::ListState>(*entry.kind);
            held.collection = new com::CollectionObject(held.state);
            held_.push_back(std::move(held));
        }
    }

    ~CollectionBehaviour() override {
        for (Held& held : held_) {
            held.collection->Release();
        }
    }

    HRESULT get(const std::string& member, const Index& index, VARIANT* result,
                long* error) override {
        for (Held& held : held_) {
            if (member != held.property) {
                continue;
            }
            if (index.count == 1U) {
                // `Tabs(1)` is `Tabs.Item(1)`.
                EXCEPINFO exception{};
                const HRESULT outcome = held.collection->item(
                    index.arguments[0], result, &exception);
                if (outcome == DISP_E_EXCEPTION) {
                    *error = exception.scode & 0xFFFF;
                    SysFreeString(exception.bstrSource);
                    SysFreeString(exception.bstrDescription);
                    return E_FAIL;
                }
                return outcome;
            }
            result->vt = VT_DISPATCH;
            result->pdispVal = held.collection;
            held.collection->AddRef();
            return S_OK;
        }
        const auto selectable = selectable_state();
        if (member == "selecteditem" && selectable != nullptr) {
            if (selectable->selected == nullptr) {
                com::detail::set_nothing(result);
                return S_OK;
            }
            result->vt = VT_DISPATCH;
            result->pdispVal =
                new com::ItemObject(selectable, selectable->selected);
            return S_OK;
        }
        return S_FALSE;
    }

    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>*) override {
        const auto state = selectable_state();
        if (member != "selecteditem" || state == nullptr) {
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
            state->selected.reset();  // Nothing
        } else {
            com::ItemObject* item = nullptr;
            if (plain.vt == VT_DISPATCH &&
                plain.pdispVal->QueryInterface(
                    com::detail::kItemIid, reinterpret_cast<void**>(&item)) ==
                    S_OK) {
                const auto& items = state->items;
                if (item->state() == state &&
                    std::find(items.begin(), items.end(), item->data()) !=
                        items.end()) {
                    state->selected = item->data();
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
    struct Held {
        std::string property;
        std::shared_ptr<com::ListState> state;
        com::CollectionObject* collection = nullptr;
    };

    [[nodiscard]] std::shared_ptr<com::ListState> selectable_state() const {
        for (const Held& held : held_) {
            if (held.state->spec->selectable) {
                return held.state;
            }
        }
        return nullptr;
    }

    std::vector<Held> held_;
};

// The behaviour of the class called `class_name`, none for most of them yet.
inline std::unique_ptr<wfc::com::Behaviour> make_behaviour(
    const char* class_name) {
    using Entry = CollectionBehaviour::Entry;
    if (_stricmp(class_name, "TabStrip") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"tabs", &com::tab_spec()}});
    }
    if (_stricmp(class_name, "StatusBar") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"panels", &com::panel_spec()}});
    }
    if (_stricmp(class_name, "Toolbar") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"buttons", &com::button_spec()}});
    }
    if (_stricmp(class_name, "TreeView") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"nodes", &com::node_spec()}});
    }
    if (_stricmp(class_name, "ListView") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"listitems", &com::list_item_spec()},
                               {"columnheaders", &com::column_spec()}});
    }
    if (_stricmp(class_name, "ImageList") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"listimages", &com::list_image_spec()}});
    }
    if (_stricmp(class_name, "ImageCombo") == 0) {
        return std::make_unique<CollectionBehaviour>(
            std::vector<Entry>{{"comboitems", &com::combo_item_spec()}});
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
