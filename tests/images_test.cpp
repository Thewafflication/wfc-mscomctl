// Checks the rules of an ImageList and an ImageCombo
// (wfc/mscomctl/images.hpp).

#include <wfc/mscomctl/images.hpp>

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

    expect(check_image_removal(true) == Status::image_list_remove_bound &&
               check_image_removal(false) == Status::ok,
           "a bound list keeps its images");
    expect(error_number(Status::image_list_remove_bound) == 35615 &&
               std::string(error_message(Status::image_list_remove_bound)) ==
                   "Images cannot be removed from a bound ImageList",
           "the removal error");

    expect(image_size_settable(0) && !image_size_settable(1),
           "the size is set only while empty");
    expect(first_image_sets_size(1) && !first_image_sets_size(0) &&
               !first_image_sets_size(2),
           "only the first image sets the size");
    expect(image_pixels(2540) == 96 && image_pixels(0) == 0 &&
               image_pixels(1270) == 48,
           "himetric to pixels at 96 dpi");
    expect(overlay_arguments_ok(2) && !overlay_arguments_ok(1) &&
               !overlay_arguments_ok(3),
           "Overlay takes two images");

    expect(same_text("Green", "GREEN") && same_text("", "") &&
               !same_text("Blu", "Blue"),
           "texts match without regard to case");
    const std::vector<std::string> texts{"Red", "green", "GREEN"};
    expect(item_with_text(texts, "Green") == std::optional<std::size_t>(1),
           "the first item with the text");
    expect(!item_with_text(texts, "gre").has_value() &&
               !item_with_text(texts, "").has_value(),
           "no item has the text");

    expect(selected_text("abcdef", 1, 3) == "bcd", "a selection");
    expect(selected_text("abcdef", 9, 4).empty(), "a start beyond the text");
    expect(selected_text("abcdef", 4, 9) == "ef", "a length beyond the text");
    expect(selected_text("abcdef", -2, -2).empty(), "negative values");
    expect(replace_selected("abcdef", 2, 2, "-") == "ab-ef", "replacing");
    expect(replace_selected("abcdef", 20, 0, "!") == "abcdef!",
           "inserting beyond the end appends");

    if (failures == 0) {
        std::puts("images: ok");
    }
    return failures == 0 ? 0 : 1;
}
