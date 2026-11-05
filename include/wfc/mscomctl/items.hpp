// The rules for finding items in the collections of the common controls (Tabs,
// Panels, Buttons, Nodes, ListItems, ColumnHeaders, ListImages, ComboItems),
// apart from any control: which index or key names an item, where an item is
// inserted, when a key is not unique, and the Visual Basic error numbers of
// the failures. Header only, with no dependency on the WFC evaluator or on
// Windows; the evaluator and the tests use it, and the COM collections will
// (ADR-0010). Internal to the WFC libraries.
//
// The rules are those of the present implementation, written from the
// documentation (there is no reference probe of the Microsoft controls yet,
// see COMPATIBILITY.md).

#ifndef WFC_MSCOMCTL_ITEMS_HPP
#define WFC_MSCOMCTL_ITEMS_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace wfc::mscomctl {

// How an operation on a collection ended.
enum class Status {
    ok,
    type_mismatch,        // an index that is neither a number nor a key
    element_not_found,    // a key that no item has
    index_out_of_bounds,  // an index outside the collection
    key_not_unique,       // a Key another item has
    panels_full,          // a status bar with sixteen panels
    image_list_bound,     // an insert in an ImageList bound to a control
    image_list_remove_bound,  // a removal from an ImageList bound to a control
};

// The Visual Basic error number of a failure (0 for ok).
[[nodiscard]] constexpr long error_number(const Status status) {
    switch (status) {
        case Status::ok:
            return 0;
        case Status::type_mismatch:
            return 13;
        case Status::element_not_found:
            return 35601;
        case Status::index_out_of_bounds:
            return 35600;
        case Status::key_not_unique:
            return 35602;
        case Status::panels_full:
            return 35609;
        case Status::image_list_bound:
        case Status::image_list_remove_bound:
            return 35615;
    }
    return 0;
}

// The text of the error.
[[nodiscard]] constexpr const char* error_message(const Status status) {
    switch (status) {
        case Status::ok:
            return "";
        case Status::type_mismatch:
            return "Type mismatch";
        case Status::element_not_found:
            return "Element not found";
        case Status::index_out_of_bounds:
            return "Index out of bounds";
        case Status::key_not_unique:
            return "Key is not unique in collection";
        case Status::panels_full:
            return "A status bar holds at most sixteen panels";
        case Status::image_list_bound:
            return "Images can only be appended to a bound ImageList";
        case Status::image_list_remove_bound:
            return "Images cannot be removed from a bound ImageList";
    }
    return "";
}

// Keys compare without regard to case (ASCII); an item without a Key is never
// found by one.
[[nodiscard]] inline bool same_key(const std::string_view stored,
                                   const std::string_view wanted) {
    if (stored.empty() || stored.size() != wanted.size()) {
        return false;
    }
    for (std::size_t i = 0; i < stored.size(); ++i) {
        auto a = static_cast<unsigned char>(stored[i]);
        auto b = static_cast<unsigned char>(wanted[i]);
        a = static_cast<unsigned char>(a >= 'A' && a <= 'Z' ? a + 32 : a);
        b = static_cast<unsigned char>(b >= 'A' && b <= 'Z' ? b + 32 : b);
        if (a != b) {
            return false;
        }
    }
    return true;
}

// What names an item: an index (a number, rounded to the nearest whole number
// as Visual Basic does), a key, or something that is neither.
class Selector {
public:
    [[nodiscard]] static Selector index(const double number) {
        Selector selector;
        selector.kind_ = Kind::number;
        selector.number_ = number;
        return selector;
    }
    [[nodiscard]] static Selector key(std::string text) {
        Selector selector;
        selector.kind_ = Kind::key;
        selector.key_ = std::move(text);
        return selector;
    }
    [[nodiscard]] static Selector invalid() { return Selector{}; }

    [[nodiscard]] bool is_key() const { return kind_ == Kind::key; }
    [[nodiscard]] bool is_index() const { return kind_ == Kind::number; }
    [[nodiscard]] const std::string& text() const { return key_; }
    [[nodiscard]] double number() const { return number_; }

private:
    enum class Kind { none, number, key };
    Kind kind_ = Kind::none;
    double number_ = 0.0;
    std::string key_;
};

// The result of looking an item up: the 0-based position when ok.
struct Lookup {
    Status status = Status::ok;
    std::size_t position = 0;
};

// Finds the item `selector` names among `count` items; `key_at(i)` is the Key
// of the item at position i.
template <class KeyAt>
[[nodiscard]] Lookup find(const std::size_t count, const Selector& selector,
                          KeyAt key_at) {
    if (selector.is_key()) {
        for (std::size_t i = 0; i < count; ++i) {
            if (same_key(key_at(i), selector.text())) {
                return {Status::ok, i};
            }
        }
        return {Status::element_not_found, 0};
    }
    if (!selector.is_index()) {
        return {Status::type_mismatch, 0};
    }
    const long index = static_cast<long>(std::nearbyint(selector.number()));
    if (index < 1 || static_cast<std::size_t>(index) > count) {
        return {Status::index_out_of_bounds, 0};
    }
    return {Status::ok, static_cast<std::size_t>(index) - 1U};
}

// Whether an item other than the one at `except` (pass `count` for none) has
// this Key. An empty Key is never in use.
template <class KeyAt>
[[nodiscard]] bool key_in_use(const std::size_t count,
                              const std::string_view key,
                              const std::size_t except, KeyAt key_at) {
    for (std::size_t i = 0; i < count; ++i) {
        if (i != except && same_key(key_at(i), key)) {
            return true;
        }
    }
    return false;
}

// Where `Add` puts an item: at the end, or at the index asked for, which may
// be one past the last item. `wanted` is null when no index was given.
[[nodiscard]] inline Lookup insert_position(const std::size_t count,
                                            const double* const wanted) {
    if (wanted == nullptr) {
        return {Status::ok, count};
    }
    const long index = static_cast<long>(std::nearbyint(*wanted));
    if (index < 1 || static_cast<std::size_t>(index) > count + 1U) {
        return {Status::index_out_of_bounds, 0};
    }
    return {Status::ok, static_cast<std::size_t>(index) - 1U};
}

// A status bar holds at most this many panels.
constexpr std::size_t kMaxPanels = 16;

// Whether a panel can be added to a status bar of `count` panels.
[[nodiscard]] inline Status check_panel_count(const std::size_t count) {
    return count >= kMaxPanels ? Status::panels_full : Status::ok;
}

// An ImageList bound to a control (its images are drawn by it) can only grow
// at the end: `position` is where an image would be inserted (0-based).
[[nodiscard]] inline Status check_image_insert(const bool bound,
                                               const std::size_t position,
                                               const std::size_t count) {
    return bound && position != count ? Status::image_list_bound : Status::ok;
}

// The first item added to a TabStrip is selected.
[[nodiscard]] inline bool selects_first_added(const std::size_t count_after) {
    return count_after == 1U;
}

// The position (0-based) of the tab that becomes selected when the selected
// tab at `position` is removed from `count` tabs: the previous one, or the
// second when the first goes; `count` itself when there is none.
[[nodiscard]] inline std::size_t neighbor_after_removal(
    const std::size_t position, const std::size_t count) {
    if (position > 0) {
        return position - 1U;
    }
    return count > 1U ? 1U : count;
}

// The order of items when a tree or a list is sorted: by text, without regard
// to case (ASCII), bytes compared as unsigned, and stable when used with
// std::stable_sort.
[[nodiscard]] inline bool text_before(const std::string_view left,
                                      const std::string_view right) {
    const auto lower = [](const char c) {
        return static_cast<unsigned char>(c >= 'A' && c <= 'Z' ? c - 'A' + 'a'
                                                               : c);
    };
    return std::lexicographical_compare(
        left.begin(), left.end(), right.begin(), right.end(),
        [&](const char a, const char b) { return lower(a) < lower(b); });
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_ITEMS_HPP
