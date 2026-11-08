// The rules of a TreeView, apart from any window, control or evaluator: where
// Nodes.Add puts a node, how siblings are sorted, the FullPath of a node, the
// rows that fit, hit testing, and what the keys do. Header only, with no
// dependency on the WFC evaluator or on Windows; the evaluator and the tests
// use it. Internal to the WFC libraries.
//
// The model works on positions: the evaluator keeps the nodes and their
// links, and asks the model what to do with counts, depths and texts. The
// metrics (a row is 240 twips high, a label 95 twips per character) are an
// approximation of the Microsoft control, as in layout.hpp.

#ifndef WFC_MSCOMCTL_TREE_HPP
#define WFC_MSCOMCTL_TREE_HPP

#include <wfc/mscomctl/items.hpp>
#include <wfc/mscomctl/labeledit.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wfc::mscomctl {

// The Relationship argument of Nodes.Add.
inline constexpr long kTreeFirst = 0;
inline constexpr long kTreeLast = 1;
inline constexpr long kTreeNext = 2;
inline constexpr long kTreePrevious = 3;
inline constexpr long kTreeChild = 4;

inline constexpr double kRowHeight = 240.0;
inline constexpr double kDefaultIndentation = 285.0;

// A Relationship is a whole number from tvwFirst to tvwChild; anything else
// is error 35607.
struct RelationshipResult {
    bool valid = true;
    long relationship = kTreeNext;
};

[[nodiscard]] inline RelationshipResult check_relationship(
    const double wanted) {
    const long relationship = static_cast<long>(std::nearbyint(wanted));
    if (relationship < kTreeFirst || relationship > kTreeChild) {
        return {false, relationship};
    }
    return {true, relationship};
}

inline constexpr int kInvalidRelationship = 35607;

// Where a new node goes among the children of its parent.
struct Placement {
    bool as_child = false;  // the relative becomes the parent
    std::size_t at = 0;     // index among the parent's children
};

// `relative_position` is the 0-based index of the relative among its own
// siblings, `siblings` their number, and `children` the number of children
// of the relative. Without a relative the node is appended as a root.
[[nodiscard]] inline Placement place_node(const bool has_relative,
                                          const long relationship,
                                          const std::size_t roots,
                                          const std::size_t children,
                                          const std::size_t siblings,
                                          const std::size_t relative_position) {
    if (!has_relative) {
        return {false, roots};
    }
    if (relationship == kTreeChild) {
        return {true, children};
    }
    switch (relationship) {
        case kTreeFirst:
            return {false, 0};
        case kTreeLast:
            return {false, siblings};
        case kTreeNext:
            return {false, relative_position + 1U};
        default:
            return {false, relative_position};
    }
}

// FullPath: the texts from the root down, joined by PathSeparator. A text
// that is empty at the root does not add a separator.
[[nodiscard]] inline std::string join_path(
    const std::vector<std::string>& root_first, const std::string& separator) {
    std::string path;
    for (const auto& part : root_first) {
        if (!path.empty()) {
            path += separator;
        }
        path += part;
    }
    return path;
}

// A node shown in the tree, in display order.
struct Row {
    int depth = 0;
    std::size_t text_length = 0;
    char first = '\0';  // first character of the text, lower case
    bool has_children = false;
    bool expanded = false;
};

// GetVisibleCount: the rows that fit in the control, at least one.
[[nodiscard]] inline std::size_t visible_count(const std::size_t rows,
                                               const double height) {
    const std::size_t fits =
        height > 0.0 ? static_cast<std::size_t>(height / kRowHeight) : rows;
    return std::min(rows, std::max<std::size_t>(fits, 1));
}

// HitTest: the index of the row under (x, y) in twips, if any.
[[nodiscard]] inline std::optional<std::size_t> hit_row(
    const std::vector<Row>& rows, const double x, const double y,
    const double indentation, const bool lines_or_buttons = true) {
    const double indent = indentation > 0.0 ? indentation : kDefaultIndentation;
    const double row = std::floor(y / kRowHeight);
    if (y < 0.0 || row < 0.0 || static_cast<std::size_t>(row) >= rows.size()) {
        return std::nullopt;
    }
    const auto& found = rows[static_cast<std::size_t>(row)];
    // Style tvwTextOnly has no room for lines or buttons: the text starts
    // 3 pixels in (measured under VB6).
    const double left = lines_or_buttons
                            ? static_cast<double>(found.depth + 1) * indent
                            : static_cast<double>(found.depth) * indent + 45.0;
    const double right =
        left + 120.0 + 95.0 * static_cast<double>(found.text_length);
    if (x >= left && x < right) {
        return static_cast<std::size_t>(row);
    }
    return std::nullopt;
}

// What a key does to a tree.
struct KeyEffect {
    enum class Kind {
        none,
        select,    // select the row `row`
        expand,    // expand the current node
        collapse,  // collapse the current node
        toggle,    // toggle the check box of the current node
    };
    Kind kind = Kind::none;
    std::size_t row = 0;
};

// `key` is lower case: "down", "up", "home", "end", "left", "right", "space",
// or a single character for type-ahead. `current` is the row of the selected
// node, or rows.size() when none is selected.
[[nodiscard]] inline KeyEffect key_effect(const std::vector<Row>& rows,
                                          const std::size_t current,
                                          const std::string_view key,
                                          const bool check_boxes) {
    using Kind = KeyEffect::Kind;
    if (rows.empty()) {
        return {};
    }
    if (current >= rows.size()) {
        return {Kind::select, 0};
    }
    const std::size_t count = rows.size();
    if (key == "down") {
        return {Kind::select, (current + 1U) % count};
    }
    if (key == "up") {
        return {Kind::select, (current + count - 1U) % count};
    }
    if (key.size() == 1U && key != " ") {
        for (std::size_t step = 1; step <= count; ++step) {
            const std::size_t candidate = (current + step) % count;
            if (rows[candidate].first != '\0' &&
                rows[candidate].first == key.front()) {
                return {Kind::select, candidate};
            }
        }
        return {};
    }
    if (key == "home") {
        return {Kind::select, 0};
    }
    if (key == "end") {
        return {Kind::select, count - 1U};
    }
    const Row& row = rows[current];
    if (key == "right") {
        if (row.has_children && !row.expanded) {
            return {Kind::expand, current};
        }
        if (row.has_children && current + 1U < count) {
            return {Kind::select, current + 1U};
        }
        return {};
    }
    if (key == "left") {
        if (row.expanded) {
            return {Kind::collapse, current};
        }
        for (std::size_t at = current; at > 0; --at) {
            if (rows[at - 1U].depth < row.depth) {
                return {Kind::select, at - 1U};
            }
        }
        return {};
    }
    if (key == "space" && check_boxes) {
        return {Kind::toggle, current};
    }
    return {};
}

// SingleSel: clicking a node collapses the node that was selected, unless it
// is an ancestor of the one clicked, and expands the one clicked.
[[nodiscard]] inline bool collapses_previous(const bool has_previous,
                                             const bool previous_is_clicked,
                                             const bool previous_is_ancestor) {
    return has_previous && !previous_is_clicked && !previous_is_ancestor;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_TREE_HPP
