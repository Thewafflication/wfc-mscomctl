// The collections of the controls of the library when a COM client creates
// them (ADR-0010, step 7): Tabs, Panels, Buttons, Nodes, ListItems,
// ColumnHeaders, ListImages and ComboItems (and the ListSubItems and
// ButtonMenus of their items) as late-bound objects (Add, Remove, Clear, Count,
// Item, _NewEnum) whose items are late-bound objects over a property bag. The
// rules (which index or key names an item, where Add inserts, the limit of
// panels, the tab that is selected after an Add or a Remove, the error numbers)
// are items.hpp, which the WFC evaluator uses too. Windows only; internal to
// the WFC libraries.
//
// The collections are returned by a Behaviour (com_behaviour.hpp) as the value
// of those properties. The events of a selection made by the program
// (BeforeClick and Click of a TabStrip, Click and Change of an ImageCombo) go
// through the raiser that the class factory hands to Behaviour::attach.

#ifndef WFC_MSCOMCTL_COM_COLLECTIONS_HPP
#define WFC_MSCOMCTL_COM_COLLECTIONS_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <ocidl.h>
#include <oleauto.h>
#include <olectl.h>

#include <wfc/mscomctl/images.hpp>
#include <wfc/mscomctl/items.hpp>
#include <wfc/mscomctl/list.hpp>
#include <wfc/mscomctl/tree.hpp>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <vector>

namespace wfc::mscomctl::com {

// What a collection holds: which properties an item has (type: s String,
// v Variant, b Boolean, i Integer, f Single), in which order Add takes its
// arguments, and whether the first added item is selected.
struct PropertySpec {
    const char* name;
    char type;
    const char* initial;  // "" for the zero value
};

struct KindSpec {
    const char* name;  // "Tab", for the message of an error
    std::vector<const char*> add_parameters;
    std::vector<PropertySpec> properties;
    bool selectable;            // one item is the selected one (SelectedItem)
    bool selects_first;         // the first item added is selected (a TabStrip)
    bool limited;               // a status bar holds at most sixteen panels
    bool needs_picture;         // Add without a picture is error 481
    bool hierarchical = false;  // Nodes: Add takes a Relative
};

[[nodiscard]] inline const KindSpec& tab_spec() {
    static const KindSpec spec{"Tab",
                               {"index", "key", "caption", "image"},
                               {{"caption", 's', ""},
                                {"key", 's', ""},
                                {"tag", 'v', ""},
                                {"image", 'v', ""},
                                {"tooltiptext", 's', ""},
                                {"visible", 'b', "1"},
                                {"highlighted", 'b', ""}},
                               true,
                               true,
                               false,
                               false};
    return spec;
}

[[nodiscard]] inline const KindSpec& panel_spec() {
    static const KindSpec spec{"Panel",
                               {"index", "key", "text", "style", "picture"},
                               {{"text", 's', ""},
                                {"key", 's', ""},
                                {"tag", 'v', ""},
                                {"style", 'i', ""},
                                {"alignment", 'i', ""},
                                {"bevel", 'i', "1"},
                                {"autosize", 'i', ""},
                                {"width", 'f', ""},
                                {"minwidth", 'f', "960"},
                                {"picture", 'v', ""},
                                {"tooltiptext", 's', ""},
                                {"visible", 'b', "1"},
                                {"enabled", 'b', "1"}},
                               false,
                               false,
                               true,
                               false};
    return spec;
}

[[nodiscard]] inline const KindSpec& column_spec() {
    static const KindSpec spec{
        "ColumnHeader",
        {"index", "key", "text", "width", "alignment", "icon"},
        {{"text", 's', ""},
         {"key", 's', ""},
         {"tag", 'v', ""},
         {"width", 'f', "1440"},
         {"alignment", 'i', ""},
         {"icon", 'v', ""},
         {"subitemindex", 'i', ""}},
        false,
        false,
        false,
        false};
    return spec;
}

[[nodiscard]] inline const KindSpec& list_item_spec() {
    static const KindSpec spec{"ListItem",
                               {"index", "key", "text", "icon", "smallicon"},
                               {{"text", 's', ""},
                                {"key", 's', ""},
                                {"tag", 'v', ""},
                                {"icon", 'v', ""},
                                {"smallicon", 'v', ""},
                                {"selected", 'b', ""},
                                {"checked", 'b', ""},
                                {"ghosted", 'b', ""},
                                {"bold", 'b', ""},
                                {"forecolor", 'l', ""},
                                {"tooltiptext", 's', ""}},
                               false,
                               false,
                               false,
                               false};
    return spec;
}

[[nodiscard]] inline const KindSpec& list_image_spec() {
    static const KindSpec spec{
        "ListImage",
        {"index", "key", "picture"},
        {{"key", 's', ""}, {"tag", 'v', ""}, {"picture", 'v', ""}},
        false,
        false,
        false,
        true};
    return spec;
}

[[nodiscard]] inline const KindSpec& combo_item_spec() {
    static const KindSpec spec{
        "ComboItem",
        {"index", "key", "text", "image", "selimage", "indentation"},
        {{"text", 's', ""},
         {"key", 's', ""},
         {"tag", 'v', ""},
         {"image", 'v', ""},
         {"selimage", 'v', ""},
         {"indentation", 'i', ""}},
        true,
        false,
        false,
        false};
    return spec;
}

[[nodiscard]] inline const KindSpec& node_spec() {
    static const KindSpec spec{
        "Node",
        {"relative", "relationship", "key", "text", "image", "selectedimage"},
        {{"text", 's', ""},
         {"key", 's', ""},
         {"tag", 'v', ""},
         {"image", 'v', ""},
         {"selectedimage", 'v', ""},
         {"expandedimage", 'v', ""},
         {"expanded", 'b', ""},
         {"checked", 'b', ""},
         {"bold", 'b', ""},
         {"forecolor", 'l', ""},
         {"backcolor", 'l', ""},
         {"sorted", 'b', ""},
         {"visible", 'b', "1"}},
        true,
        false,
        false,
        false,
        true};
    return spec;
}

[[nodiscard]] inline const KindSpec& button_spec() {
    static const KindSpec spec{"Button",
                               {"index", "key", "caption", "style", "image"},
                               {{"caption", 's', ""},
                                {"key", 's', ""},
                                {"tag", 'v', ""},
                                {"style", 'i', ""},
                                {"value", 'i', ""},
                                {"image", 'v', ""},
                                {"tooltiptext", 's', ""},
                                {"description", 's', ""},
                                {"visible", 'b', "1"},
                                {"enabled", 'b', "1"},
                                {"mixedstate", 'b', ""},
                                {"width", 'f', ""}},
                               false,
                               false,
                               false,
                               false};
    return spec;
}

[[nodiscard]] inline const KindSpec& list_sub_item_spec() {
    static const KindSpec spec{
        "ListSubItem",
        {"index", "key", "text", "reporticon", "tooltiptext"},
        {{"text", 's', ""},
         {"key", 's', ""},
         {"tag", 'v', ""},
         {"reporticon", 'v', ""},
         {"forecolor", 'l', ""},
         {"bold", 'b', ""},
         {"tooltiptext", 's', ""}},
        false,
        false,
        false,
        false};
    return spec;
}

[[nodiscard]] inline const KindSpec& button_menu_spec() {
    static const KindSpec spec{"ButtonMenu",
                               {"index", "key", "text"},
                               {{"text", 's', ""},
                                {"key", 's', ""},
                                {"tag", 'v', ""},
                                {"enabled", 'b', "1"},
                                {"visible", 'b', "1"}},
                               false,
                               false,
                               false,
                               false};
    return spec;
}

// The collection an item of this kind holds (a list item's ListSubItems, a
// button's ButtonMenus), or null.
[[nodiscard]] inline const KindSpec* sub_spec_of(const KindSpec& spec) {
    const std::string name = spec.name;
    return name == "ListItem" ? &list_sub_item_spec()
           : name == "Button" ? &button_menu_spec()
                              : nullptr;
}

[[nodiscard]] inline const char* sub_property_of(const KindSpec& spec) {
    return std::string(spec.name) == "ListItem" ? "listsubitems"
                                                : "buttonmenus";
}

// What a selection raises: 't' BeforeClick (cancellable) and Click, as a
// TabStrip; 'c' Click and Change, as an ImageCombo; 0 nothing.
[[nodiscard]] inline char event_style_of(const KindSpec& spec) {
    const std::string name = spec.name;
    return name == "Tab" ? 't' : name == "ComboItem" ? 'c' : '\0';
}

struct ListState;

// One item: its properties by lower-case name.
struct ItemData {
    std::map<std::string, VARIANT> properties;
    ItemData* parent = nullptr;  // a node's parent (the items own the nodes)
    std::vector<ItemData*> kids;
    std::shared_ptr<ListState> sub;  // ListSubItems or ButtonMenus

    ItemData() = default;
    ItemData(const ItemData&) = delete;
    ItemData& operator=(const ItemData&) = delete;
    ~ItemData() {
        for (auto& entry : properties) {
            VariantClear(&entry.second);
        }
    }
};

// The items of one collection, shared by the collection, its items and the
// control (for SelectedItem).
struct ListState {
    explicit ListState(const KindSpec& kind_spec) : spec(&kind_spec) {}

    const KindSpec* spec;
    std::vector<std::shared_ptr<ItemData>> items;
    std::vector<ItemData*> roots;  // the nodes without a parent, in order
    std::shared_ptr<ItemData> selected;

    // Set by Behaviour::attach: raises an event of the control with the
    // arguments given (in the order of the call).
    std::function<void(const std::string&, std::vector<VARIANT>&)> raise;

    // The properties of the control that its input depends on (the control
    // stores them; the Behaviour hears them as they are set).
    double label_edit = 0.0;  // 0 automatic, 1 manual
    bool multi_select = false;
    bool single_sel = false;
    bool checkboxes = false;
    bool allow_customize = true;
    bool locked = false;
    bool editing = false;  // a label edit has started
    bool bound = false;    // an ImageList drawn by a control
    bool sorted = false;   // the Sorted property of a TreeView
    std::string path_separator = "\\";
    short image_width = 0;  // an ImageList: set by its first image
    short image_height = 0;
    std::string text;  // an ImageCombo: Text, SelStart and SelLength
    double sel_start = 0.0;
    double sel_length = 0.0;

    // Raises an event without arguments, or (cancellable) with Cancel; true
    // when a sink set Cancel.
    bool emit(const char* name, const bool cancellable = false) const {
        if (!raise) {
            return false;
        }
        SHORT cancel = 0;
        std::vector<VARIANT> arguments;
        if (cancellable) {
            VARIANT argument;
            VariantInit(&argument);
            argument.vt = VT_I2 | VT_BYREF;
            argument.piVal = &cancel;
            arguments.push_back(argument);
        }
        raise(name, arguments);
        return cancel != 0;
    }

    void emit_with(const char* name, std::vector<VARIANT>& arguments) const {
        if (raise) {
            raise(name, arguments);
        }
    }

    [[nodiscard]] std::shared_ptr<ItemData> share(const ItemData* data) const {
        for (const auto& item : items) {
            if (item.get() == data) {
                return item;
            }
        }
        return nullptr;
    }

    // Takes `node` and everything below it out of the collection.
    void drop_subtree(ItemData* node) {
        std::vector<ItemData*> doomed{node};
        for (std::size_t i = 0; i < doomed.size(); ++i) {
            doomed.insert(doomed.end(), doomed[i]->kids.begin(),
                          doomed[i]->kids.end());
        }
        auto& siblings = node->parent != nullptr ? node->parent->kids : roots;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), node),
                       siblings.end());
        for (ItemData* gone : doomed) {
            if (selected.get() == gone) {
                selected.reset();
            }
            gone->parent = nullptr;
            gone->kids.clear();
            items.erase(std::remove_if(items.begin(), items.end(),
                                       [&](const std::shared_ptr<ItemData>& i) {
                                           return i.get() == gone;
                                       }),
                        items.end());
        }
    }
};

namespace detail {

inline std::string narrow(const wchar_t* text) {
    std::string result;
    for (; text != nullptr && *text != L'\0'; ++text) {
        result.push_back(*text < 128 ? static_cast<char>(*text) : '?');
    }
    return result;
}

[[nodiscard]] inline std::string lower(std::string text) {
    for (char& c : text) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return text;
}

// A failure the Visual Basic client sees as error `number`.
inline HRESULT fail(EXCEPINFO* exception, const long number,
                    const char* message) {
    if (exception != nullptr) {
        *exception = EXCEPINFO{};
        exception->scode = static_cast<SCODE>(
            0x800A0000UL | static_cast<unsigned long>(number));
        exception->bstrSource = SysAllocString(L"WFC");
        std::wstring text;
        for (const char* c = message; *c != '\0'; ++c) {
            text.push_back(static_cast<wchar_t>(*c));
        }
        exception->bstrDescription = SysAllocString(text.c_str());
    }
    return DISP_E_EXCEPTION;
}

inline HRESULT fail(EXCEPINFO* exception, const Status status) {
    return fail(exception, error_number(status), error_message(status));
}

[[nodiscard]] inline bool missing(const VARIANT& value) {
    return value.vt == VT_ERROR && value.scode == DISP_E_PARAMNOTFOUND;
}

[[nodiscard]] inline bool is_number(const VARIANT& value) {
    switch (value.vt & VT_TYPEMASK) {
        case VT_I1:
        case VT_I2:
        case VT_I4:
        case VT_I8:
        case VT_UI1:
        case VT_UI2:
        case VT_UI4:
        case VT_R4:
        case VT_R8:
        case VT_CY:
        case VT_BOOL:
            return true;
        default:
            return false;
    }
}

// The argument `value` as an item selector: a number, a key, or neither.
[[nodiscard]] inline Selector selector_of(const VARIANT& value) {
    VARIANT plain;
    VariantInit(&plain);
    if (VariantCopyInd(&plain, &value) != S_OK) {
        return Selector::invalid();
    }
    Selector selector = Selector::invalid();
    if (plain.vt == VT_BSTR) {
        selector = Selector::key(narrow(plain.bstrVal));
    } else if (is_number(plain)) {
        VARIANT number;
        VariantInit(&number);
        if (VariantChangeType(&number, &plain, 0, VT_R8) == S_OK) {
            selector = Selector::index(number.dblVal);
        }
    }
    VariantClear(&plain);
    return selector;
}

[[nodiscard]] inline std::string key_of(const ItemData& item) {
    const auto found = item.properties.find("key");
    return found != item.properties.end() && found->second.vt == VT_BSTR
               ? narrow(found->second.bstrVal)
               : std::string{};
}

// Converts `value` for a property of type `type` into `out`; false when it
// cannot be (a type mismatch).
[[nodiscard]] inline bool coerce(const char type, const VARIANT& value,
                                 VARIANT* out) {
    VariantInit(out);
    VARTYPE wanted = VT_EMPTY;
    switch (type) {
        case 's':
            wanted = VT_BSTR;
            break;
        case 'b':
            wanted = VT_BOOL;
            break;
        case 'i':
            wanted = VT_I2;
            break;
        case 'l':
            wanted = VT_I4;
            break;
        case 'f':
            wanted = VT_R4;
            break;
        default:
            return VariantCopyInd(out, &value) == S_OK;
    }
    VARIANT plain;
    VariantInit(&plain);
    if (VariantCopyInd(&plain, &value) != S_OK) {
        return false;
    }
    const bool ok = VariantChangeType(out, &plain, 0, wanted) == S_OK;
    VariantClear(&plain);
    return ok;
}

inline void initial_value(const PropertySpec& property, VARIANT* out) {
    VariantInit(out);
    switch (property.type) {
        case 's':
            out->vt = VT_BSTR;
            out->bstrVal = SysAllocString(L"");
            break;
        case 'b':
            out->vt = VT_BOOL;
            out->boolVal =
                property.initial[0] == '1' ? VARIANT_TRUE : VARIANT_FALSE;
            break;
        case 'i':
            out->vt = VT_I2;
            out->iVal = property.initial[0] == '\0'
                            ? static_cast<SHORT>(0)
                            : static_cast<SHORT>(std::stol(property.initial));
            break;
        case 'l':
            out->vt = VT_I4;
            out->lVal = 0;
            break;
        case 'f':
            out->vt = VT_R4;
            out->fltVal = property.initial[0] == '\0'
                              ? 0.0F
                              : std::stof(property.initial);
            break;
        default:
            break;  // Empty
    }
}

[[nodiscard]] inline const PropertySpec* find_property(
    const KindSpec& spec, const std::string& name) {
    for (const PropertySpec& property : spec.properties) {
        if (name == property.name) {
            return &property;
        }
    }
    return nullptr;
}

[[nodiscard]] inline std::string text_of(const ItemData& item,
                                         const char* name) {
    const auto found = item.properties.find(name);
    return found != item.properties.end() && found->second.vt == VT_BSTR
               ? narrow(found->second.bstrVal)
               : std::string{};
}

[[nodiscard]] inline bool flag_of(const ItemData& item, const char* name) {
    const auto found = item.properties.find(name);
    return found != item.properties.end() && found->second.vt == VT_BOOL &&
           found->second.boolVal != VARIANT_FALSE;
}

[[nodiscard]] inline bool to_long(const VARIANT& value, long* out) {
    VARIANT plain;
    VARIANT number;
    VariantInit(&plain);
    VariantInit(&number);
    bool ok = false;
    if (VariantCopyInd(&plain, &value) == S_OK && is_number(plain) &&
        VariantChangeType(&number, &plain, 0, VT_R8) == S_OK) {
        *out = static_cast<long>(std::nearbyint(number.dblVal));
        ok = true;
    }
    VariantClear(&plain);
    return ok;
}

inline void set_nothing(VARIANT* result) {
    VariantInit(result);
    result->vt = VT_DISPATCH;
    result->pdispVal = nullptr;
}

// The IID by which an item hands out its data to the collection of its own
// library (a private interface, never registered).
inline const GUID kItemIid = {0x6f0b3f20,
                              0x5c0e,
                              0x4a1e,
                              {0x9a, 0x6d, 0x31, 0x4c, 0x77, 0x0e, 0x2b, 0x91}};

inline const GUID kCollectionIid = {
    0x6f0b3f21,
    0x5c0e,
    0x4a1e,
    {0x9a, 0x6d, 0x31, 0x4c, 0x77, 0x0e, 0x2b, 0x92}};

// A new item with the initial value of each property (and its sub
// collection, if its kind has one).
[[nodiscard]] inline std::shared_ptr<ItemData> make_item(const KindSpec& spec) {
    auto item = std::make_shared<ItemData>();
    for (const PropertySpec& property : spec.properties) {
        initial_value(property, &item->properties[property.name]);
    }
    if (const KindSpec* sub = sub_spec_of(spec)) {
        item->sub = std::make_shared<ListState>(*sub);
    }
    return item;
}

}  // namespace detail

// Sorts siblings by their text, as a sorted TreeView does (stable).
inline void sort_siblings(std::vector<ItemData*>& kids) {
    std::stable_sort(kids.begin(), kids.end(),
                     [](const ItemData* left, const ItemData* right) {
                         return text_before(detail::text_of(*left, "text"),
                                            detail::text_of(*right, "text"));
                     });
}

// Selects `item` as the program does (SelectedItem, Selected = True): a
// TabStrip raises BeforeClick, which can cancel, and Click; an ImageCombo
// takes the item's text and raises Click and Change.
inline void select_item_notify(ListState& state,
                               const std::shared_ptr<ItemData>& item) {
    const char style = event_style_of(*state.spec);
    if (style == 't') {
        if (state.selected == item) {
            return;
        }
        if (state.emit("BeforeClick", true)) {
            state.emit("Click");  // the selection stays, Click still runs
            return;
        }
        state.selected = item;
        state.emit("Click");
        return;
    }
    state.selected = item;
    if (style == 'c') {
        state.text = detail::text_of(*item, "text");
        state.emit("Click");
        state.emit("Change");
    }
}

// The first image of an ImageList gives the list its size, in pixels.
inline void measure_image(ListState& state, const VARIANT& picture) {
    VARIANT plain;
    VariantInit(&plain);
    if (VariantCopyInd(&plain, &picture) == S_OK && plain.vt == VT_DISPATCH &&
        plain.pdispVal != nullptr) {
        IPicture* drawing = nullptr;
        if (plain.pdispVal->QueryInterface(
                IID_IPicture, reinterpret_cast<void**>(&drawing)) == S_OK) {
            OLE_XSIZE_HIMETRIC width = 0;
            OLE_YSIZE_HIMETRIC height = 0;
            drawing->get_Width(&width);
            drawing->get_Height(&height);
            state.image_width = image_pixels(static_cast<long>(width));
            state.image_height = image_pixels(static_cast<long>(height));
            drawing->Release();
        }
    }
    VariantClear(&plain);
}

// A late-bound object that finds its members by name.
class LateObject : public IDispatch {
public:
    HRESULT __stdcall QueryInterface(REFIID iid, void** object) override {
        if (object == nullptr) {
            return E_POINTER;
        }
        if (IsEqualIID(iid, IID_IUnknown) || IsEqualIID(iid, IID_IDispatch)) {
            *object = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override {
        return static_cast<ULONG>(++references_);
    }
    ULONG __stdcall Release() override {
        const long left = --references_;
        if (left == 0) {
            delete this;
        }
        return static_cast<ULONG>(left);
    }
    HRESULT __stdcall GetTypeInfoCount(UINT* count) override {
        if (count == nullptr) {
            return E_POINTER;
        }
        *count = 0;
        return S_OK;
    }
    HRESULT __stdcall GetTypeInfo(UINT, LCID, ITypeInfo** info) override {
        if (info != nullptr) {
            *info = nullptr;
        }
        return E_NOTIMPL;
    }

protected:
    LateObject() = default;
    virtual ~LateObject() = default;

private:
    std::atomic<long> references_{1};
};

// One item of a collection.
class ItemObject final : public LateObject {
public:
    ItemObject(std::shared_ptr<ListState> state, std::shared_ptr<ItemData> data)
        : state_(std::move(state)), data_(std::move(data)) {}

    [[nodiscard]] const std::shared_ptr<ItemData>& data() const {
        return data_;
    }
    [[nodiscard]] const std::shared_ptr<ListState>& state() const {
        return state_;
    }

    HRESULT __stdcall QueryInterface(REFIID iid, void** object) override {
        if (object != nullptr && IsEqualIID(iid, detail::kItemIid)) {
            *object = this;
            AddRef();
            return S_OK;
        }
        return LateObject::QueryInterface(iid, object);
    }

    HRESULT __stdcall GetIDsOfNames(REFIID, LPOLESTR* names, UINT count, LCID,
                                    DISPID* ids) override {
        if (names == nullptr || ids == nullptr) {
            return E_POINTER;
        }
        HRESULT result = S_OK;
        for (UINT i = 0; i < count; ++i) {
            ids[i] = DISPID_UNKNOWN;
            if (i != 0) {
                result = DISP_E_UNKNOWNNAME;
                continue;
            }
            const std::string name = detail::lower(detail::narrow(names[i]));
            if (name == "index" || name == "selected" ||
                (state_->spec->hierarchical && is_derived(name)) ||
                (data_->sub != nullptr &&
                 (name == sub_property_of(*state_->spec) ||
                  name == "subitems")) ||
                detail::find_property(*state_->spec, name) != nullptr) {
                ids_.push_back(name);
                ids[i] = static_cast<DISPID>(ids_.size());
            } else {
                result = DISP_E_UNKNOWNNAME;
            }
        }
        return result;
    }

    HRESULT __stdcall Invoke(DISPID id, REFIID, LCID, WORD flags,
                             DISPPARAMS* parameters, VARIANT* result,
                             EXCEPINFO* exception, UINT*) override {
        if (id < 1 || static_cast<std::size_t>(id) > ids_.size()) {
            return DISP_E_MEMBERNOTFOUND;
        }
        const std::string& name = ids_[static_cast<std::size_t>(id) - 1U];
        if ((flags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)) != 0) {
            if (parameters == nullptr || parameters->cArgs < 1) {
                return DISP_E_BADPARAMCOUNT;
            }
            if (name == "subitems") {
                return put_sub_item(parameters, exception);
            }
            return put(name, parameters->rgvarg[0], exception);
        }
        if (result == nullptr) {
            return S_OK;
        }
        VariantInit(result);
        if (data_->sub != nullptr && name == sub_property_of(*state_->spec)) {
            return sub_collection(result);
        }
        if (name == "subitems") {
            return get_sub_item(parameters, result, exception);
        }
        if (name == "index") {
            result->vt = VT_I4;
            result->lVal = static_cast<LONG>(position() + 1U);
            return S_OK;
        }
        if (state_->spec->hierarchical && is_derived(name)) {
            return derived(name, result);
        }
        if (name == "selected" && state_->spec->selectable) {
            result->vt = VT_BOOL;
            result->boolVal =
                state_->selected == data_ ? VARIANT_TRUE : VARIANT_FALSE;
            return S_OK;
        }
        const auto found = data_->properties.find(name);
        return found == data_->properties.end()
                   ? S_OK
                   : VariantCopy(result, &found->second);
    }

private:
    // The ListSubItems or ButtonMenus of the item (defined below).
    HRESULT sub_collection(VARIANT* result);

    // SubItems(Index): 0 is the text of the item itself (list.hpp rules).
    HRESULT get_sub_item(DISPPARAMS* parameters, VARIANT* result,
                         EXCEPINFO* exception) {
        long index = 0;
        if (parameters == nullptr || parameters->cArgs != 1U ||
            !detail::to_long(parameters->rgvarg[0], &index)) {
            return detail::fail(exception, Status::type_mismatch);
        }
        const auto& sub = data_->sub->items;
        const Status status = check_sub_item_read(index, sub.size());
        if (status != Status::ok) {
            return detail::fail(exception, status);
        }
        const ItemData& source =
            index == 0 ? *data_ : *sub[static_cast<std::size_t>(index) - 1U];
        return VariantCopy(result, &source.properties.at("text"));
    }

    HRESULT put_sub_item(DISPPARAMS* parameters, EXCEPINFO* exception) {
        long index = 0;
        VARIANT text;
        if (parameters->cArgs != 2U ||
            !detail::to_long(parameters->rgvarg[1], &index) ||
            !detail::coerce('s', parameters->rgvarg[0], &text)) {
            return detail::fail(exception, Status::type_mismatch);
        }
        const Status status = check_sub_item_write(index);
        if (status != Status::ok) {
            VariantClear(&text);
            return detail::fail(exception, status);
        }
        auto& sub = data_->sub->items;
        for (std::size_t missing = sub_items_to_add(index, sub.size());
             missing > 0; --missing) {
            sub.push_back(detail::make_item(*data_->sub->spec));
        }
        ItemData& target =
            index == 0 ? *data_ : *sub[static_cast<std::size_t>(index) - 1U];
        VariantClear(&target.properties["text"]);
        target.properties["text"] = text;
        return S_OK;
    }

    [[nodiscard]] static bool is_derived(const std::string& name) {
        return name == "parent" || name == "children" || name == "child" ||
               name == "next" || name == "previous" || name == "firstsibling" ||
               name == "lastsibling" || name == "root" || name == "fullpath";
    }

    // The links of a node, read from the tree (Nothing where there is none).
    HRESULT derived(const std::string& name, VARIANT* result) {
        const ItemData* node = data_.get();
        const auto& siblings =
            node->parent != nullptr ? node->parent->kids : state_->roots;
        const auto where = std::find(siblings.begin(), siblings.end(), node);
        const auto give = [&](const ItemData* other) {
            const auto shared = other != nullptr ? state_->share(other)
                                                 : std::shared_ptr<ItemData>();
            if (shared == nullptr) {
                detail::set_nothing(result);
            } else {
                result->vt = VT_DISPATCH;
                result->pdispVal = new ItemObject(state_, shared);
            }
            return S_OK;
        };
        if (name == "parent") {
            return give(node->parent);
        }
        if (name == "children") {
            result->vt = VT_I2;
            result->iVal = static_cast<SHORT>(node->kids.size());
            return S_OK;
        }
        if (name == "child") {
            return give(node->kids.empty() ? nullptr : node->kids.front());
        }
        if (name == "next") {
            return give(where != siblings.end() && where + 1 != siblings.end()
                            ? *(where + 1)
                            : nullptr);
        }
        if (name == "previous") {
            return give(where != siblings.end() && where != siblings.begin()
                            ? *(where - 1)
                            : nullptr);
        }
        if (name == "firstsibling") {
            return give(siblings.empty() ? nullptr : siblings.front());
        }
        if (name == "lastsibling") {
            return give(siblings.empty() ? nullptr : siblings.back());
        }
        if (name == "root") {
            const ItemData* at = node;
            while (at->parent != nullptr) {
                at = at->parent;
            }
            return give(at);
        }
        std::vector<std::string> parts;  // fullpath
        for (const ItemData* at = node; at != nullptr; at = at->parent) {
            const auto text = at->properties.find("text");
            parts.insert(
                parts.begin(),
                text != at->properties.end() && text->second.vt == VT_BSTR
                    ? detail::narrow(text->second.bstrVal)
                    : std::string{});
        }
        const std::string path = join_path(parts, state_->path_separator);
        result->vt = VT_BSTR;
        const std::wstring wide(path.begin(), path.end());
        result->bstrVal = SysAllocString(wide.c_str());
        return S_OK;
    }

    [[nodiscard]] std::size_t position() const {
        for (std::size_t i = 0; i < state_->items.size(); ++i) {
            if (state_->items[i] == data_) {
                return i;
            }
        }
        return state_->items.size();
    }

    HRESULT put(const std::string& name, const VARIANT& value,
                EXCEPINFO* exception) {
        if (name == "index" ||
            (state_->spec->hierarchical && is_derived(name))) {
            return detail::fail(exception, 383, "Property is read-only");
        }
        if (name == "selected" && !state_->spec->selectable) {
            // A list item: Selected = True replaces the selection unless the
            // control is MultiSelect (list.hpp).
            VARIANT flag;
            if (!detail::coerce('b', value, &flag)) {
                return detail::fail(exception, Status::type_mismatch);
            }
            if (flag.boolVal != VARIANT_FALSE &&
                selected_replaces(state_->multi_select)) {
                for (const auto& other : state_->items) {
                    VariantClear(&other->properties["selected"]);
                    other->properties["selected"].vt = VT_BOOL;
                    other->properties["selected"].boolVal = VARIANT_FALSE;
                }
            }
            VariantClear(&data_->properties["selected"]);
            data_->properties["selected"] = flag;
            return S_OK;
        }
        if (name == "selected") {
            VARIANT flag;
            if (!detail::coerce('b', value, &flag)) {
                return detail::fail(exception, Status::type_mismatch);
            }
            if (flag.boolVal != VARIANT_FALSE && state_->spec->selectable) {
                select_item_notify(*state_, data_);
            }
            return S_OK;
        }
        const PropertySpec* property =
            detail::find_property(*state_->spec, name);
        if (property == nullptr) {
            return DISP_E_MEMBERNOTFOUND;
        }
        if (name == "key" && value.vt != VT_BSTR &&
            value.vt != (VT_BSTR | VT_BYREF)) {
            return detail::fail(exception, Status::type_mismatch);
        }
        VARIANT converted;
        if (!detail::coerce(property->type, value, &converted)) {
            return detail::fail(exception, Status::type_mismatch);
        }
        if (name == "key" && converted.bstrVal != nullptr &&
            *converted.bstrVal != L'\0') {
            const std::string key = detail::narrow(converted.bstrVal);
            const std::size_t self = position();
            if (key_in_use(state_->items.size(), key, self,
                           [&](const std::size_t i) {
                               return detail::key_of(*state_->items[i]);
                           })) {
                VariantClear(&converted);
                return detail::fail(exception, Status::key_not_unique);
            }
        }
        VariantClear(&data_->properties[name]);
        data_->properties[name] = converted;
        if (name == "sorted" && state_->spec->hierarchical &&
            converted.boolVal != VARIANT_FALSE) {
            sort_siblings(data_->kids);
        }
        return S_OK;
    }

    std::shared_ptr<ListState> state_;
    std::shared_ptr<ItemData> data_;
    std::vector<std::string> ids_;
};

// IEnumVARIANT over the items present when For Each starts.
class ItemEnumerator final : public IEnumVARIANT {
public:
    explicit ItemEnumerator(std::vector<IDispatch*> items, std::size_t at = 0)
        : items_(std::move(items)), at_(at) {}

    ~ItemEnumerator() {
        for (IDispatch* item : items_) {
            item->Release();
        }
    }

    HRESULT __stdcall QueryInterface(REFIID iid, void** object) override {
        if (object == nullptr) {
            return E_POINTER;
        }
        if (IsEqualIID(iid, IID_IUnknown) ||
            IsEqualIID(iid, IID_IEnumVARIANT)) {
            *object = static_cast<IEnumVARIANT*>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG __stdcall AddRef() override {
        return static_cast<ULONG>(++references_);
    }
    ULONG __stdcall Release() override {
        const long left = --references_;
        if (left == 0) {
            delete this;
        }
        return static_cast<ULONG>(left);
    }
    HRESULT __stdcall Next(ULONG wanted, VARIANT* out,
                           ULONG* fetched) override {
        ULONG done = 0;
        while (done < wanted && at_ < items_.size()) {
            VariantInit(&out[done]);
            out[done].vt = VT_DISPATCH;
            out[done].pdispVal = items_[at_];
            items_[at_]->AddRef();
            ++at_;
            ++done;
        }
        if (fetched != nullptr) {
            *fetched = done;
        }
        return done == wanted ? S_OK : S_FALSE;
    }
    HRESULT __stdcall Skip(ULONG count) override {
        at_ = std::min<std::size_t>(items_.size(), at_ + count);
        return at_ < items_.size() ? S_OK : S_FALSE;
    }
    HRESULT __stdcall Reset() override {
        at_ = 0;
        return S_OK;
    }
    HRESULT __stdcall Clone(IEnumVARIANT** clone) override {
        if (clone == nullptr) {
            return E_POINTER;
        }
        std::vector<IDispatch*> copy = items_;
        for (IDispatch* item : copy) {
            item->AddRef();
        }
        *clone = new (std::nothrow) ItemEnumerator(std::move(copy), at_);
        return *clone != nullptr ? S_OK : E_OUTOFMEMORY;
    }

private:
    std::atomic<long> references_{1};
    std::vector<IDispatch*> items_;
    std::size_t at_;
};

// The collection: Add, Remove, Clear, Count, Item and _NewEnum.
class CollectionObject final : public LateObject {
public:
    explicit CollectionObject(std::shared_ptr<ListState> state)
        : state_(std::move(state)) {}

    [[nodiscard]] const std::shared_ptr<ListState>& state() const {
        return state_;
    }

    HRESULT __stdcall QueryInterface(REFIID iid, void** object) override {
        if (object != nullptr && IsEqualIID(iid, detail::kCollectionIid)) {
            *object = this;
            AddRef();
            return S_OK;
        }
        return LateObject::QueryInterface(iid, object);
    }

    // The item `selector` names, as an object; an error leaves `exception`.
    HRESULT item(const VARIANT& selector, VARIANT* result,
                 EXCEPINFO* exception) {
        const auto lookup = find_in(detail::selector_of(selector));
        if (lookup.status != Status::ok) {
            return detail::fail(exception, lookup.status);
        }
        return wrap(state_->items[lookup.position], result);
    }

    HRESULT __stdcall GetIDsOfNames(REFIID, LPOLESTR* names, UINT count, LCID,
                                    DISPID* ids) override {
        if (names == nullptr || ids == nullptr) {
            return E_POINTER;
        }
        HRESULT result = S_OK;
        for (UINT i = 0; i < count; ++i) {
            ids[i] = DISPID_UNKNOWN;
            if (i != 0) {
                result = DISP_E_UNKNOWNNAME;
                continue;
            }
            const std::string name = detail::lower(detail::narrow(names[i]));
            if (name == "add") {
                ids[i] = kAdd;
            } else if (name == "remove") {
                ids[i] = kRemove;
            } else if (name == "clear") {
                ids[i] = kClear;
            } else if (name == "count") {
                ids[i] = kCount;
            } else if (name == "item") {
                ids[i] = DISPID_VALUE;
            } else if (name == "_newenum" || name == "[_newenum]") {
                ids[i] = DISPID_NEWENUM;
            } else {
                result = DISP_E_UNKNOWNNAME;
            }
        }
        return result;
    }

    HRESULT __stdcall Invoke(DISPID id, REFIID, LCID, WORD,
                             DISPPARAMS* parameters, VARIANT* result,
                             EXCEPINFO* exception, UINT*) override {
        if (result != nullptr) {
            VariantInit(result);
        }
        const UINT count = parameters != nullptr ? parameters->cArgs : 0;
        // The arguments come last first.
        const auto argument = [&](const UINT position) -> const VARIANT& {
            return parameters->rgvarg[count - 1U - position];
        };
        switch (id) {
            case kCount:
                if (result != nullptr) {
                    result->vt = VT_I4;
                    result->lVal = static_cast<LONG>(state_->items.size());
                }
                return S_OK;
            case DISPID_VALUE:
                if (count != 1U) {
                    return detail::fail(exception, 450,
                                        "Wrong number of arguments");
                }
                if (result != nullptr) {
                    return item(argument(0), result, exception);
                } else {
                    VARIANT unused;
                    VariantInit(&unused);
                    const HRESULT outcome =
                        item(argument(0), &unused, exception);
                    VariantClear(&unused);
                    return outcome;
                }
            case kClear:
                if (const Status status = removal_status();
                    status != Status::ok) {
                    return detail::fail(exception, status);
                }
                for (const auto& item : state_->items) {
                    item->parent = nullptr;
                    item->kids.clear();
                }
                state_->items.clear();
                state_->roots.clear();
                state_->selected.reset();
                return S_OK;
            case kRemove:
                if (count != 1U) {
                    return detail::fail(exception, 450,
                                        "Wrong number of arguments");
                }
                return remove(argument(0), exception);
            case kAdd:
                return add(count, parameters, result, exception);
            case DISPID_NEWENUM:
                return enumerate(result);
            default:
                return DISP_E_MEMBERNOTFOUND;
        }
    }

private:
    static constexpr DISPID kAdd = 1;
    static constexpr DISPID kRemove = 2;
    static constexpr DISPID kClear = 3;
    static constexpr DISPID kCount = 4;

    [[nodiscard]] Lookup find_in(const Selector& selector) const {
        return find(state_->items.size(), selector, [&](const std::size_t i) {
            return detail::key_of(*state_->items[i]);
        });
    }

    HRESULT wrap(const std::shared_ptr<ItemData>& data, VARIANT* result) {
        auto* object = new (std::nothrow) ItemObject(state_, data);
        if (object == nullptr) {
            return E_OUTOFMEMORY;
        }
        result->vt = VT_DISPATCH;
        result->pdispVal = object;
        return S_OK;
    }

    // Remove and Clear of the images of an ImageList that a control draws.
    [[nodiscard]] Status removal_status() const {
        return state_->spec->needs_picture ? check_image_removal(state_->bound)
                                           : Status::ok;
    }

    HRESULT remove(const VARIANT& selector, EXCEPINFO* exception) {
        if (const Status status = removal_status(); status != Status::ok) {
            return detail::fail(exception, status);
        }
        const auto lookup = find_in(detail::selector_of(selector));
        if (lookup.status != Status::ok) {
            return detail::fail(exception, lookup.status);
        }
        const auto removed = state_->items[lookup.position];
        if (state_->spec->hierarchical) {
            state_->drop_subtree(removed.get());  // and everything below it
            return S_OK;
        }
        if (state_->selected == removed) {
            // A tab hands the selection to its neighbour; other items leave
            // nothing selected.
            const std::size_t count = state_->items.size();
            const std::size_t next =
                neighbor_after_removal(lookup.position, count);
            state_->selected = state_->spec->selects_first && next < count
                                   ? state_->items[next]
                                   : nullptr;
        }
        state_->items.erase(state_->items.begin() +
                            static_cast<std::ptrdiff_t>(lookup.position));
        return S_OK;
    }

    HRESULT add(const UINT count, DISPPARAMS* parameters, VARIANT* result,
                EXCEPINFO* exception) {
        const KindSpec& spec = *state_->spec;
        if (count > spec.add_parameters.size() ||
            (parameters != nullptr && parameters->cNamedArgs != 0U)) {
            return detail::fail(exception, 450, "Wrong number of arguments");
        }
        std::map<std::string, const VARIANT*> given;
        for (UINT i = 0; i < count; ++i) {
            const VARIANT& value = parameters->rgvarg[count - 1U - i];
            if (!detail::missing(value)) {
                given[spec.add_parameters[i]] = &value;
            }
        }
        std::size_t position = state_->items.size();
        ItemData* parent = nullptr;
        std::size_t at = 0;
        if (spec.hierarchical) {
            const HRESULT placed = place(given, &parent, &at, exception);
            if (placed != S_OK) {
                return placed;
            }
        } else if (const auto index = given.find("index");
                   index != given.end()) {
            VARIANT plain;
            VARIANT number;
            VariantInit(&plain);
            VariantInit(&number);
            const bool numeric =
                VariantCopyInd(&plain, index->second) == S_OK &&
                detail::is_number(plain) &&
                VariantChangeType(&number, &plain, 0, VT_R8) == S_OK;
            VariantClear(&plain);
            if (!numeric) {
                return detail::fail(exception, Status::type_mismatch);
            }
            const double wanted = number.dblVal;
            const auto where = insert_position(state_->items.size(), &wanted);
            if (where.status != Status::ok) {
                return detail::fail(exception, where.status);
            }
            position = where.position;
            if (spec.needs_picture) {
                const Status status = check_image_insert(
                    state_->bound, position, state_->items.size());
                if (status != Status::ok) {
                    return detail::fail(exception, status);
                }
            }
        }
        if (spec.limited) {
            const Status status = check_panel_count(state_->items.size());
            if (status != Status::ok) {
                return detail::fail(exception, status);
            }
        }
        auto created = detail::make_item(spec);
        for (const auto& [name, value] : given) {
            if (name == "index" || name == "relative" ||
                name == "relationship") {
                continue;
            }
            const PropertySpec* property = detail::find_property(spec, name);
            if (property == nullptr) {
                return detail::fail(exception, 450,
                                    "Wrong number of arguments");
            }
            if (name == "key" && value->vt != VT_BSTR &&
                value->vt != (VT_BSTR | VT_BYREF)) {
                return detail::fail(exception, Status::type_mismatch);
            }
            VARIANT converted;
            if (!detail::coerce(property->type, *value, &converted)) {
                return detail::fail(exception, Status::type_mismatch);
            }
            VariantClear(&created->properties[name]);
            created->properties[name] = converted;
        }
        const std::string key = detail::key_of(*created);
        if (!key.empty() &&
            key_in_use(state_->items.size(), key, state_->items.size(),
                       [&](const std::size_t i) {
                           return detail::key_of(*state_->items[i]);
                       })) {
            return detail::fail(exception, Status::key_not_unique);
        }
        if (spec.needs_picture) {
            const VARIANT& picture = created->properties["picture"];
            if (picture.vt == VT_EMPTY || picture.vt == VT_NULL ||
                (picture.vt == VT_DISPATCH && picture.pdispVal == nullptr)) {
                return detail::fail(exception, 481, "Invalid picture");
            }
        }
        if (spec.hierarchical) {
            created->parent = parent;
            auto& siblings = parent != nullptr ? parent->kids : state_->roots;
            siblings.insert(siblings.begin() + static_cast<std::ptrdiff_t>(at),
                            created.get());
            state_->items.push_back(created);
            if (state_->sorted ||
                (parent != nullptr && detail::flag_of(*parent, "sorted"))) {
                sort_siblings(siblings);
            }
        } else {
            state_->items.insert(
                state_->items.begin() + static_cast<std::ptrdiff_t>(position),
                created);
        }
        if (spec.selects_first && selects_first_added(state_->items.size())) {
            state_->selected = created;
        }
        if (spec.needs_picture && first_image_sets_size(state_->items.size())) {
            measure_image(*state_, created->properties["picture"]);
        }
        if (result == nullptr) {
            return S_OK;
        }
        return wrap(created, result);
    }

    // Where Nodes.Add puts the node: the parent and the place among its kids
    // (tree.hpp). Without a Relative the node is a new root; the
    // Relationship defaults to tvwNext.
    HRESULT place(const std::map<std::string, const VARIANT*>& given,
                  ItemData** parent, std::size_t* at, EXCEPINFO* exception) {
        const auto relative_value = given.find("relative");
        if (relative_value == given.end()) {
            *parent = nullptr;
            *at = state_->roots.size();
            return S_OK;
        }
        ItemData* relative = nullptr;
        VARIANT plain;
        VariantInit(&plain);
        if (VariantCopyInd(&plain, relative_value->second) != S_OK) {
            return detail::fail(exception, Status::type_mismatch);
        }
        if (plain.vt == VT_DISPATCH) {
            ItemObject* object = nullptr;
            if (plain.pdispVal != nullptr &&
                plain.pdispVal->QueryInterface(
                    detail::kItemIid, reinterpret_cast<void**>(&object)) ==
                    S_OK) {
                if (object->state() == state_ &&
                    state_->share(object->data().get()) != nullptr) {
                    relative = object->data().get();
                }
                object->Release();
            }
            VariantClear(&plain);
            if (relative == nullptr) {
                return detail::fail(exception, Status::element_not_found);
            }
        } else {
            VariantClear(&plain);
            const auto lookup =
                find_in(detail::selector_of(*relative_value->second));
            if (lookup.status != Status::ok) {
                return detail::fail(exception, lookup.status);
            }
            relative = state_->items[lookup.position].get();
        }
        long relationship = kTreeNext;
        if (const auto way = given.find("relationship"); way != given.end()) {
            VARIANT number;
            VARIANT value;
            VariantInit(&value);
            VariantInit(&number);
            const bool numeric =
                VariantCopyInd(&value, way->second) == S_OK &&
                detail::is_number(value) &&
                VariantChangeType(&number, &value, 0, VT_R8) == S_OK;
            VariantClear(&value);
            if (!numeric) {
                return detail::fail(exception, Status::type_mismatch);
            }
            const auto checked = check_relationship(number.dblVal);
            if (!checked.valid) {
                return detail::fail(exception, kInvalidRelationship,
                                    "Invalid relationship");
            }
            relationship = checked.relationship;
        }
        const auto& siblings = relative->parent != nullptr
                                   ? relative->parent->kids
                                   : state_->roots;
        const auto where =
            std::find(siblings.begin(), siblings.end(), relative);
        const auto placement = place_node(
            true, relationship, 0, relative->kids.size(), siblings.size(),
            static_cast<std::size_t>(where - siblings.begin()));
        *parent = placement.as_child ? relative : relative->parent;
        *at = placement.at;
        return S_OK;
    }

    HRESULT enumerate(VARIANT* result) {
        if (result == nullptr) {
            return S_OK;
        }
        std::vector<IDispatch*> objects;
        for (const auto& data : state_->items) {
            objects.push_back(new ItemObject(state_, data));
        }
        auto* enumerator =
            new (std::nothrow) ItemEnumerator(std::move(objects));
        if (enumerator == nullptr) {
            return E_OUTOFMEMORY;
        }
        result->vt = VT_UNKNOWN;
        result->punkVal = enumerator;
        return S_OK;
    }

    std::shared_ptr<ListState> state_;
};

// An item as the argument of an event (Nothing for none); VariantClear
// releases it.
[[nodiscard]] inline VARIANT item_variant(
    const std::shared_ptr<ListState>& state,
    const std::shared_ptr<ItemData>& item) {
    VARIANT value;
    VariantInit(&value);
    if (item == nullptr) {
        detail::set_nothing(&value);
    } else {
        value.vt = VT_DISPATCH;
        value.pdispVal = new ItemObject(state, item);
    }
    return value;
}

inline HRESULT ItemObject::sub_collection(VARIANT* result) {
    auto* collection = new (std::nothrow) CollectionObject(data_->sub);
    if (collection == nullptr) {
        return E_OUTOFMEMORY;
    }
    result->vt = VT_DISPATCH;
    result->pdispVal = collection;
    return S_OK;
}

}  // namespace wfc::mscomctl::com

#endif  // WFC_MSCOMCTL_COM_COLLECTIONS_HPP
