// Checks the rules of the buttons of a toolbar (wfc/mscomctl/toolbar.hpp):
// valid styles and values, what a press does, and the saved layout.

#include <wfc/mscomctl/toolbar.hpp>

#include <cstdio>
#include <string>

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

    expect(valid_button_style(0) && valid_button_style(5) &&
               !valid_button_style(-1) && !valid_button_style(6),
           "styles run from 0 to 5");
    expect(valid_button_value(0) && valid_button_value(1) &&
               !valid_button_value(-1) && !valid_button_value(2),
           "values are 0 and 1");

    expect(press_of(kButtonDefault, false) == Press::ignore,
           "a disabled button ignores a press");
    expect(press_of(kButtonSeparator, true) == Press::ignore &&
               press_of(kButtonPlaceholder, true) == Press::ignore,
           "separators and placeholders ignore a press");
    expect(press_of(kButtonDefault, true) == Press::plain &&
               press_of(kButtonDropdown, true) == Press::plain,
           "default and drop-down buttons only click");
    expect(press_of(kButtonCheck, true) == Press::toggle, "check toggles");
    expect(press_of(kButtonGroup, true) == Press::group, "group presses");
    expect(toggled_value(0) == 1 && toggled_value(1) == 0, "toggle flips");

    expect(value_presses_group(kButtonGroup, 1) &&
               !value_presses_group(kButtonGroup, 0) &&
               !value_presses_group(kButtonCheck, 1),
           "Value = tbrPressed presses only a group button");
    expect(drops_down(kButtonDropdown, true) &&
               !drops_down(kButtonDropdown, false) &&
               !drops_down(kButtonDefault, true),
           "only an enabled drop-down button drops down");

    const std::vector<SavedButton> buttons{{"new", true}, {"open", false}};
    const std::string text = save_layout(buttons);
    expect(text == "new=1;open=0;", "the saved text");
    const auto back = parse_layout(text);
    expect(back.size() == 2 && back[0].key == "new" && back[0].visible &&
               back[1].key == "open" && !back[1].visible,
           "the saved text reads back");
    const auto odd = parse_layout("a=1;junk;b=0");
    expect(odd.size() == 2 && odd[0].key == "a" && odd[1].key == "b" &&
               !odd[1].visible,
           "a piece without = is skipped and the last needs no ;");
    expect(parse_layout("").empty(), "an empty layout has no entries");

    if (failures == 0) {
        std::puts("toolbar: ok");
    }
    return failures == 0 ? 0 : 1;
}
