// The collections of the controls of the library when a COM client creates
// them (ADR-0010, step 7): TabStrip.Tabs, StatusBar.Panels and Toolbar.Buttons
// as late-bound objects (Add, Remove, Clear, Count, Item, _NewEnum) whose items
// are late-bound objects over a property bag. The rules (which index or key
// names an item, where Add inserts, the limit of panels, the tab that is
// selected after an Add or a Remove, the error numbers) are items.hpp, which
// the WFC evaluator uses too. Windows only; internal to the WFC libraries.
//
// The collections are returned by a Behaviour (com_behaviour.hpp) as the value
// of the Tabs, Panels and Buttons properties, so the class factory needs no
// change. Events (BeforeClick, Click) are not raised here yet: they are the
// two-step operations of ADR-0010 decision 4.

#ifndef WFC_MSCOMCTL_COM_COLLECTIONS_HPP
#define WFC_MSCOMCTL_COM_COLLECTIONS_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <oleauto.h>

#include <wfc/mscomctl/items.hpp>

#include <algorithm>
#include <atomic>
#include <cstddef>
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
    bool selectable;  // the collection of a TabStrip
    bool limited;     // a status bar holds at most sixteen panels
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
                               false};
    return spec;
}

// One item: its properties by lower-case name.
struct ItemData {
    std::map<std::string, VARIANT> properties;

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
    std::shared_ptr<ItemData> selected;
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

}  // namespace detail

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
            return put(name, parameters->rgvarg[0], exception);
        }
        if (result == nullptr) {
            return S_OK;
        }
        VariantInit(result);
        if (name == "index") {
            result->vt = VT_I4;
            result->lVal = static_cast<LONG>(position() + 1U);
            return S_OK;
        }
        if (name == "selected") {
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
        if (name == "index") {
            return detail::fail(exception, 383, "Property is read-only");
        }
        if (name == "selected") {
            VARIANT flag;
            if (!detail::coerce('b', value, &flag)) {
                return detail::fail(exception, Status::type_mismatch);
            }
            if (flag.boolVal != VARIANT_FALSE && state_->spec->selectable) {
                state_->selected = data_;
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
                state_->items.clear();
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

    HRESULT remove(const VARIANT& selector, EXCEPINFO* exception) {
        const auto lookup = find_in(detail::selector_of(selector));
        if (lookup.status != Status::ok) {
            return detail::fail(exception, lookup.status);
        }
        const auto removed = state_->items[lookup.position];
        if (state_->selected == removed) {
            const std::size_t count = state_->items.size();
            const std::size_t next =
                neighbor_after_removal(lookup.position, count);
            state_->selected = next < count ? state_->items[next] : nullptr;
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
        if (const auto index = given.find("index"); index != given.end()) {
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
        }
        if (spec.limited) {
            const Status status = check_panel_count(state_->items.size());
            if (status != Status::ok) {
                return detail::fail(exception, status);
            }
        }
        auto created = std::make_shared<ItemData>();
        for (const PropertySpec& property : spec.properties) {
            detail::initial_value(property,
                                  &created->properties[property.name]);
        }
        for (const auto& [name, value] : given) {
            if (name == "index") {
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
        state_->items.insert(
            state_->items.begin() + static_cast<std::ptrdiff_t>(position),
            created);
        if (spec.selectable && selects_first_added(state_->items.size())) {
            state_->selected = created;
        }
        if (result == nullptr) {
            return S_OK;
        }
        return wrap(created, result);
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

}  // namespace wfc::mscomctl::com

#endif  // WFC_MSCOMCTL_COM_COLLECTIONS_HPP
