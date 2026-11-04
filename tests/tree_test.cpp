// Checks the rules of a tree (wfc/mscomctl/tree.hpp): where a node is added,
// sorting, paths, rows that fit, hit testing and the keys.

#include <wfc/mscomctl/tree.hpp>

#include <cstdio>
#include <string>
#include <vector>

namespace {

int failures = 0;

void expect(const bool condition, const char* what) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

}  // namespace

int main() {
    using namespace wfc::mscomctl;
    using Kind = KeyEffect::Kind;

    expect(check_relationship(0).valid && check_relationship(4).valid &&
               check_relationship(2.5).relationship == 2,
           "relationships 0 to 4, rounded");
    expect(!check_relationship(5).valid && !check_relationship(-1).valid,
           "relationships outside 0 to 4");
    expect(kInvalidRelationship == 35607, "the error number");

    // Three roots; the relative is the second (position 1) with two children.
    auto placed = place_node(false, kTreeChild, 3, 0, 0, 0);
    expect(!placed.as_child && placed.at == 3, "no relative: a new root");
    placed = place_node(true, kTreeChild, 3, 2, 3, 1);
    expect(placed.as_child && placed.at == 2, "child: after the last child");
    placed = place_node(true, kTreeFirst, 3, 2, 3, 1);
    expect(!placed.as_child && placed.at == 0, "first sibling");
    placed = place_node(true, kTreeLast, 3, 2, 3, 1);
    expect(placed.at == 3, "last sibling");
    placed = place_node(true, kTreeNext, 3, 2, 3, 1);
    expect(placed.at == 2, "after the relative");
    placed = place_node(true, kTreePrevious, 3, 2, 3, 1);
    expect(placed.at == 1, "before the relative");

    expect(text_before("alpha", "Zulu") && !text_before("Zulu", "alpha") &&
               !text_before("Same", "same") && !text_before("same", "SAME"),
           "sorting ignores case");
    expect(text_before("a", "ab") && !text_before("ab", "a"),
           "a prefix comes first");

    expect(join_path({"Root", "Kid"}, "\\") == "Root\\Kid", "a path");
    expect(join_path({"", "Kid"}, "/") == "Kid", "an empty root adds nothing");
    expect(join_path({"Only"}, "/") == "Only", "a root alone");

    expect(visible_count(10, 1000.0) == 4 && visible_count(2, 1000.0) == 2 &&
               visible_count(10, 100.0) == 1 && visible_count(10, 0.0) == 10,
           "rows that fit");

    const std::vector<Row> rows{{0, 4, 'r', true, true},
                                {1, 5, 'k', true, true},
                                {2, 3, 'g', false, false},
                                {0, 6, 's', false, false}};
    expect(hit_row(rows, 400, 10, 0) == std::optional<std::size_t>(0),
           "the first row");
    expect(hit_row(rows, 700, 250, 0) == std::optional<std::size_t>(1),
           "the second row, indented once more");
    expect(!hit_row(rows, 100, 10, 0).has_value(), "left of the label");
    expect(!hit_row(rows, 400, 960, 0).has_value(), "below the last row");
    expect(!hit_row(rows, 400, -5, 0).has_value(), "above the first row");
    expect(hit_row(rows, 600, 10, 500) == std::optional<std::size_t>(0),
           "a wider indentation moves the label");

    const auto effect = [&](const std::size_t current, const char* key,
                            const bool boxes = false) {
        return key_effect(rows, current, key, boxes);
    };
    expect(effect(4, "down").kind == Kind::select && effect(4, "down").row == 0,
           "nothing selected: the first row");
    expect(effect(3, "down").row == 0, "down wraps");
    expect(effect(0, "up").row == 3, "up wraps");
    expect(effect(1, "home").row == 0 && effect(1, "end").row == 3,
           "home and end");
    expect(effect(0, "s").row == 3 && effect(3, "s").row == 3,
           "type-ahead finds the next row, or the same one");
    expect(effect(0, "z").kind == Kind::none, "type-ahead without a match");
    expect(effect(0, "right").row == 1, "right goes to the first child");
    expect(
        effect(2, "left").row == 1 && effect(1, "left").kind == Kind::collapse,
        "left collapses, then goes to the parent");
    expect(effect(3, "left").kind == Kind::none, "left on a root");
    expect(effect(2, "space", true).kind == Kind::toggle &&
               effect(2, "space", false).kind == Kind::none,
           "space toggles a check box only with check boxes");
    std::vector<Row> collapsed{{0, 4, 'r', true, false}};
    expect(key_effect(collapsed, 0, "right", false).kind == Kind::expand,
           "right expands a collapsed node");
    expect(key_effect({}, 0, "down", false).kind == Kind::none,
           "an empty tree");

    expect(collapses_previous(true, false, false) &&
               !collapses_previous(true, true, false) &&
               !collapses_previous(true, false, true) &&
               !collapses_previous(false, false, false),
           "SingleSel collapses the previous node");
    expect(click_starts_edit(true, 0.0) && !click_starts_edit(true, 1.0) &&
               !click_starts_edit(false, 0.0),
           "a second click starts an automatic edit");
    expect(keeps_edited_text(true, false) && !keeps_edited_text(true, true) &&
               !keeps_edited_text(false, false),
           "the edited text is kept");

    if (failures == 0) {
        std::puts("tree: ok");
    }
    return failures == 0 ? 0 : 1;
}
