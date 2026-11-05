// The behaviour of the controls of the library when a COM client creates them
// (see wfc/ocx/class_factory.hpp). The rules themselves are the library's
// models, which the WFC evaluator uses too. Windows only; internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL_COM_BEHAVIOUR_HPP
#define WFC_MSCOMCTL_COM_BEHAVIOUR_HPP

#include "com_collections.hpp"

#include <wfc/mscomctl/images.hpp>
#include <wfc/mscomctl/range.hpp>
#include <wfc/ocx/class_factory.hpp>

#include <algorithm>
#include <functional>
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
// Toolbar.Buttons, TreeView.Nodes, ListView.ListItems and ColumnHeaders,
// ImageList.ListImages, ImageCombo.ComboItems): the property of that name
// answers the collection, which is late bound (com_collections.hpp). A control
// whose items can be selected (a TabStrip, a TreeView, an ImageCombo) also
// keeps SelectedItem and raises the events of a selection made by the program;
// an ImageCombo keeps Text, SelStart, SelLength and SelText with the selection;
// a TreeView keeps Sorted and PathSeparator; an ImageList keeps its size; and
// a control given an ImageList binds it, so its images can then only grow.
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

    void attach(std::function<bool(const std::string&, bool)> raiser) override {
        for (Held& held : held_) {
            held.state->raise = raiser;
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
        if (const auto combo = state_of_kind("ComboItem")) {
            if (member == "text") {
                return text_result(combo->text, result);
            }
            if (member == "seltext") {
                return text_result(selected_text(combo->text, combo->sel_start,
                                                 combo->sel_length),
                                   result);
            }
            if (member == "selstart" || member == "sellength") {
                result->vt = VT_I4;
                result->lVal =
                    static_cast<LONG>(member == "selstart" ? combo->sel_start
                                                           : combo->sel_length);
                return S_OK;
            }
        }
        if (const auto images = state_of_kind("ListImage");
            images != nullptr && images->image_width > 0 &&
            (member == "imagewidth" || member == "imageheight")) {
            result->vt = VT_I2;
            result->iVal = member == "imagewidth" ? images->image_width
                                                  : images->image_height;
            return S_OK;
        }
        return S_FALSE;
    }

    HRESULT put(const std::string& member, const Index&, const VARIANT& value,
                long* error, std::vector<std::string>* events) override {
        if (member == "imagelist" || member == "icons" ||
            member == "smallicons" || member == "hotimagelist" ||
            member == "disabledimagelist" || member == "columnheadericons") {
            bind_image_list(value);  // and the control stores it as usual
            return S_FALSE;
        }
        if (const auto tree = state_of_kind("Node")) {
            if (member == "pathseparator") {
                VARIANT text;
                if (com::detail::coerce('s', value, &text)) {
                    tree->path_separator = com::detail::narrow(text.bstrVal);
                    VariantClear(&text);
                }
                return S_FALSE;
            }
            if (member == "sorted") {
                VARIANT flag;
                if (com::detail::coerce('b', value, &flag)) {
                    tree->sorted = flag.boolVal != VARIANT_FALSE;
                    if (tree->sorted) {
                        com::sort_siblings(tree->roots);
                        for (const auto& node : tree->items) {
                            com::sort_siblings(node->kids);
                        }
                    }
                }
                return S_FALSE;
            }
        }
        if (const auto images = state_of_kind("ListImage");
            images != nullptr &&
            (member == "imagewidth" || member == "imageheight")) {
            if (!image_size_settable(images->items.size())) {
                *error = 35614;  // read-only after images are added
                return E_FAIL;
            }
            return S_FALSE;
        }
        if (const auto combo = state_of_kind("ComboItem")) {
            const HRESULT handled = put_combo(*combo, member, value, events);
            if (handled != S_FALSE) {
                return handled;
            }
        }
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
                    com::select_item_notify(*state, item->data());
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

    [[nodiscard]] std::shared_ptr<com::ListState> state_of_kind(
        const char* name) const {
        for (const Held& held : held_) {
            if (std::string(held.state->spec->name) == name) {
                return held.state;
            }
        }
        return nullptr;
    }

    static HRESULT text_result(const std::string& text, VARIANT* result) {
        const std::wstring wide(text.begin(), text.end());
        result->vt = VT_BSTR;
        result->bstrVal = SysAllocString(wide.c_str());
        return S_OK;
    }

    // Text, SelStart, SelLength and SelText of an ImageCombo (images.hpp).
    static HRESULT put_combo(com::ListState& combo, const std::string& member,
                             const VARIANT& value,
                             std::vector<std::string>* events) {
        if (member == "selstart" || member == "sellength") {
            long number = 0;
            if (com::detail::to_long(value, &number)) {
                (member == "selstart" ? combo.sel_start : combo.sel_length) =
                    static_cast<double>(number);
            }
            return S_OK;
        }
        if (member != "text" && member != "seltext") {
            return S_FALSE;
        }
        VARIANT converted;
        if (!com::detail::coerce('s', value, &converted)) {
            return S_FALSE;
        }
        const std::string wanted = com::detail::narrow(converted.bstrVal);
        VariantClear(&converted);
        if (member == "seltext") {
            combo.text = replace_selected(combo.text, combo.sel_start,
                                          combo.sel_length, wanted);
            combo.sel_length = static_cast<double>(wanted.size());
            events->push_back("Change");
            return S_OK;
        }
        const bool changed = combo.text != wanted;
        combo.text = wanted;
        // The selection follows the text when an item has exactly that text.
        std::vector<std::string> texts;
        for (const auto& item : combo.items) {
            texts.push_back(com::detail::text_of(*item, "text"));
        }
        const auto match = item_with_text(texts, wanted);
        combo.selected = match.has_value() ? combo.items[*match] : nullptr;
        if (changed) {
            events->push_back("Change");
        }
        return S_OK;
    }

    // An ImageList given to a control: its images can then only grow. The
    // list is found through its own ListImages property.
    static void bind_image_list(const VARIANT& value) {
        VARIANT plain;
        VariantInit(&plain);
        if (VariantCopyInd(&plain, &value) != S_OK) {
            return;
        }
        if (plain.vt == VT_DISPATCH && plain.pdispVal != nullptr) {
            OLECHAR name[] = L"ListImages";
            OLECHAR* names[1] = {name};
            DISPID id = 0;
            DISPPARAMS none = {nullptr, nullptr, 0, 0};
            VARIANT images;
            VariantInit(&images);
            if (plain.pdispVal->GetIDsOfNames(IID_NULL, names, 1, 0, &id) ==
                    S_OK &&
                plain.pdispVal->Invoke(id, IID_NULL, 0, DISPATCH_PROPERTYGET,
                                       &none, &images, nullptr,
                                       nullptr) == S_OK &&
                images.vt == VT_DISPATCH && images.pdispVal != nullptr) {
                com::CollectionObject* collection = nullptr;
                if (images.pdispVal->QueryInterface(
                        com::detail::kCollectionIid,
                        reinterpret_cast<void**>(&collection)) == S_OK) {
                    collection->state()->bound = true;
                    collection->Release();
                }
            }
            VariantClear(&images);
        }
        VariantClear(&plain);
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
