// Checks the rules of a list (wfc/mscomctl/list.hpp): views, positions, hit
// testing, sorting, FindItem, SubItems, selection and label editing.

#include <wfc/mscomctl/list.hpp>

#include <cstdio>
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

    expect(valid_view(0) && valid_view(3) && !valid_view(-1) && !valid_view(4),
           "views run from 0 to 3");

    ListOptions report;
    report.view = kViewReport;
    report.width = 3000;
    report.height = 1200;
    report.columns_width = 1700;
    auto rects = list_layout(3, report);
    expect(rects.size() == 3 && rects[0].top == 240 && rects[1].top == 480 &&
               rects[0].width == 1700 && rects[0].height == 240,
           "report rows sit below the header");
    report.hide_headers = true;
    rects = list_layout(2, report);
    expect(rects[0].top == 0 && rects[1].top == 240, "no header, no offset");

    ListOptions icons;
    icons.view = kViewIcon;
    icons.width = 3000;
    rects = list_layout(4, icons);
    expect(rects[1].left == 1200 && rects[2].left == 0 && rects[2].top == 1100,
           "icons wrap at the width");
    ListOptions list;
    list.view = kViewList;
    list.width = 3000;
    list.height = 500;
    rects = list_layout(5, list);
    expect(rects[1].top == 240 && rects[2].top == 0 && rects[2].left == 1440,
           "list view fills columns downwards");
    list.height = 0;
    rects = list_layout(1, list);
    expect(rects.size() == 1, "an unset height still lays out");

    rects = list_layout(3, report);
    expect(hit_rect(rects, 10, 250) == std::optional<std::size_t>(1),
           "the second row");
    expect(hit_rect(rects, 10, 10) == std::optional<std::size_t>(0),
           "no header: the first row starts at the top");
    expect(!hit_rect(rects, 1700, 250).has_value(), "right of the rows");
    expect(!hit_rect(rects, 10, 5000).has_value(), "below the rows");

    expect(sort_source(0, 2) == std::optional<std::size_t>(0) &&
               sort_source(-3, 2) == std::optional<std::size_t>(0),
           "sort by the item itself");
    expect(sort_source(2, 2) == std::optional<std::size_t>(2) &&
               !sort_source(3, 2).has_value(),
           "sort by a sub item, or by nothing");
    expect(list_precedes("alpha", "Bravo", false) &&
               list_precedes("Bravo", "alpha", true) &&
               !list_precedes("Same", "same", false) &&
               !list_precedes("Same", "same", true),
           "ascending and descending ignore case");

    expect(find_matches("Alpha", "alpha", false) &&
               !find_matches("Alphabet", "alpha", false),
           "a whole match ignores case");
    expect(find_matches("Alphabet", "ALPHA", true) &&
               !find_matches("Alp", "alpha", true) &&
               find_matches("anything", "", true),
           "a partial match is a prefix");
    expect(find_matches("", "", false), "empty matches empty");

    expect(check_sub_item_read(0, 2) == Status::ok &&
               check_sub_item_read(2, 2) == Status::ok &&
               check_sub_item_read(3, 2) == Status::index_out_of_bounds &&
               check_sub_item_read(-1, 2) == Status::index_out_of_bounds,
           "reading sub items");
    expect(check_sub_item_write(0) == Status::ok &&
               check_sub_item_write(-1) == Status::index_out_of_bounds,
           "writing sub items");
    expect(sub_items_to_add(3, 0) == 3 && sub_items_to_add(2, 2) == 0 &&
               sub_items_to_add(0, 2) == 0,
           "sub items added up to the index");

    std::vector<bool> chosen{false, true, false};
    select_item(chosen, 2, false);
    expect(!chosen[0] && !chosen[1] && chosen[2], "a click replaces");
    select_item(chosen, 0, true);
    expect(chosen[0] && chosen[2], "Ctrl adds");
    select_item(chosen, 0, true);
    expect(!chosen[0] && chosen[2], "Ctrl toggles");
    expect(selected_replaces(false) && !selected_replaces(true),
           "Selected replaces without MultiSelect");
    expect(is_second_click(true, false) && !is_second_click(true, true) &&
               !is_second_click(false, false),
           "a second click");
    expect(click_extends(true, true) && !click_extends(true, false) &&
               !click_extends(false, true),
           "Ctrl extends only with MultiSelect");

    expect(click_starts_edit(true, 0.0) && !click_starts_edit(true, 1.0),
           "automatic label editing");
    expect(scripted_edit_may_start(true, 1.0) &&
               scripted_edit_may_start(false, 0.0) &&
               !scripted_edit_may_start(false, 1.0),
           "a scripted edit starts unless manual");
    expect(keeps_edited_text(true, false) && !keeps_edited_text(true, true),
           "the edited text is kept");
    expect(text_before("a", "b") && !text_before("\xC3", "A"),
           "bytes compare as unsigned");

    if (failures == 0) {
        std::puts("list: ok");
    }
    return failures == 0 ? 0 : 1;
}
