// The rules of a ListView, apart from any window, control or evaluator: the
// valid views, the positions of the items in each view, hit testing, sorting
// by a sub item, FindItem, SubItems and the selection. Header only, with no
// dependency on the WFC evaluator or on Windows; the evaluator and the tests
// use it. Internal to the WFC libraries.
//
// The metrics (a row of report view is 240 twips high, an icon cell 1200 by
// 1100) are an approximation of the Microsoft control, as in layout.hpp.

#ifndef WFC_MSCOMCTL_LIST_HPP
#define WFC_MSCOMCTL_LIST_HPP

#include <wfc/mscomctl/items.hpp>
#include <wfc/mscomctl/labeledit.hpp>
#include <wfc/mscomctl/layout.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace wfc::mscomctl {

// ListView.View: lvwIcon, lvwSmallIcon, lvwList, lvwReport.
inline constexpr long kViewIcon = 0;
inline constexpr long kViewSmallIcon = 1;
inline constexpr long kViewList = 2;
inline constexpr long kViewReport = 3;

[[nodiscard]] constexpr bool valid_view(const double view) {
    return view >= 0.0 && view <= 3.0;
}

// What the positions of the items depend on.
struct ListOptions {
    long view = kViewIcon;
    double width = 0;
    double height = 0;
    bool hide_headers = false;
    double columns_width = 0;  // the sum of the widths of the column headers
};

// The rectangle of each of `count` items, in twips.
[[nodiscard]] inline std::vector<Rect> list_layout(const std::size_t count,
                                                   const ListOptions& options) {
    std::vector<Rect> rects;
    const bool icons = options.view == kViewIcon;
    const double header =
        options.view == kViewReport && !options.hide_headers ? 240.0 : 0.0;
    const double cell_width = icons ? 1200.0 : 1440.0;
    const double cell_height = icons ? 1100.0 : 240.0;
    const long per_row =
        std::max<long>(1, static_cast<long>(options.width / cell_width));
    const long per_column = std::max<long>(
        1, static_cast<long>((options.height > 0 ? options.height : 2400.0) /
                             cell_height));
    for (std::size_t i = 0; i < count; ++i) {
        const long n = static_cast<long>(i);
        if (options.view == kViewReport) {
            rects.push_back({0.0, header + 240.0 * static_cast<double>(n),
                             options.columns_width, 240.0});
        } else if (options.view == kViewList) {
            rects.push_back({cell_width * static_cast<double>(n / per_column),
                             cell_height * static_cast<double>(n % per_column),
                             cell_width, cell_height});
        } else {
            rects.push_back({cell_width * static_cast<double>(n % per_row),
                             cell_height * static_cast<double>(n / per_row),
                             cell_width, cell_height});
        }
    }
    return rects;
}

// HitTest: the first rectangle that holds (x, y).
[[nodiscard]] inline std::optional<std::size_t> hit_rect(
    const std::vector<Rect>& rects, const double x, const double y) {
    for (std::size_t i = 0; i < rects.size(); ++i) {
        const Rect& rect = rects[i];
        if (x >= rect.left && x < rect.left + rect.width && y >= rect.top &&
            y < rect.top + rect.height) {
            return i;
        }
    }
    return std::nullopt;
}

// Sorting: SortKey 0 sorts by the text of the item, SortKey n by the text of
// sub item n. `sub_count` is the number of sub items of the item. The result
// is the sub item to read (1-based), 0 for the item itself, or nullopt when
// the item has no such sub item (it sorts as an empty text).
[[nodiscard]] constexpr std::optional<std::size_t> sort_source(
    const long sort_key, const std::size_t sub_count) {
    if (sort_key <= 0) {
        return std::size_t{0};
    }
    if (static_cast<std::size_t>(sort_key) <= sub_count) {
        return static_cast<std::size_t>(sort_key);
    }
    return std::nullopt;
}

// SortOrder: lvwAscending (0) or lvwDescending (1).
[[nodiscard]] inline bool list_precedes(const std::string_view left,
                                        const std::string_view right,
                                        const bool descending) {
    return descending ? text_before(right, left) : text_before(left, right);
}

// FindItem(String, Where, Index, Match): where to look.
inline constexpr long kFindText = 0;
inline constexpr long kFindSubItem = 1;
inline constexpr long kFindTag = 2;

// Whether `candidate` matches `wanted` without regard to case, as a whole or
// (lvwPartial) as a prefix.
[[nodiscard]] inline bool find_matches(const std::string_view candidate,
                                       const std::string_view wanted,
                                       const bool partial) {
    const std::string_view head =
        partial ? candidate.substr(0, wanted.size()) : candidate;
    return !text_before(head, wanted) && !text_before(wanted, head);
}

// SubItems(Index): 0 is the text of the item itself.
[[nodiscard]] constexpr Status check_sub_item_read(
    const long index, const std::size_t sub_count) {
    return index < 0 || static_cast<std::size_t>(index) > sub_count
               ? Status::index_out_of_bounds
               : Status::ok;
}

// Writing a sub item beyond the last one adds empty ones up to it.
[[nodiscard]] constexpr Status check_sub_item_write(const long index) {
    return index < 0 ? Status::index_out_of_bounds : Status::ok;
}

[[nodiscard]] constexpr std::size_t sub_items_to_add(
    const long index, const std::size_t sub_count) {
    return index > 0 && static_cast<std::size_t>(index) > sub_count
               ? static_cast<std::size_t>(index) - sub_count
               : std::size_t{0};
}

// Selecting an item: without `extend` it is the only one selected; with
// `extend` (Ctrl) it is toggled and the others stay as they are.
inline void select_item(std::vector<bool>& selected, const std::size_t item,
                        const bool extend) {
    for (std::size_t i = 0; i < selected.size(); ++i) {
        if (i == item) {
            selected[i] = extend ? !selected[i] : true;
        } else if (!extend) {
            selected[i] = false;
        }
    }
}

// Selected = True on an item replaces the selection unless MultiSelect.
[[nodiscard]] constexpr bool selected_replaces(const bool multi_select) {
    return !multi_select;
}

// A second click on a selected item (without Ctrl) may start a label edit.
[[nodiscard]] constexpr bool is_second_click(const bool was_selected,
                                             const bool ctrl) {
    return was_selected && !ctrl;
}

// Ctrl extends the selection only with MultiSelect.
[[nodiscard]] constexpr bool click_extends(const bool ctrl,
                                           const bool multi_select) {
    return ctrl && multi_select;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_LIST_HPP
