// Checks the layout of the tabs of a TabStrip and the buttons of a Toolbar
// (wfc/mscomctl/layout.hpp): default sizes, fixed widths, wrapping, hidden
// items and the groups of buttons.

#include <cmath>
#include <wfc/mscomctl/layout.hpp>

#include <cstdio>

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

    // Tabs: width follows the text (pixels), in the container's coordinates
    // (2 pixels inside the control).
    std::vector<TabSpec> tabs{
        {true, 1, 6.0}, {true, 10, 50.0}, {false, 5, 25.0}, {true, 0, 0.0}};
    auto rects = tab_layout(tabs, TabOptions{});
    expect(rects.size() == 4, "one rectangle per tab");
    expect(rects[0].left == twips_of_pixels(2.0) &&
               rects[0].top == twips_of_pixels(2.0) &&
               rects[0].width == twips_of_pixels(19.0) &&
               rects[0].height == twips_of_pixels(18.0),
           "a tab is its text plus 13 pixels");
    expect(rects[1].left == twips_of_pixels(21.0) &&
               rects[1].width == twips_of_pixels(63.0),
           "a long caption widens the tab");
    expect(rects[2].width == 0.0 && rects[2].height == 0.0,
           "a hidden tab is empty");
    expect(rects[3].left == twips_of_pixels(84.0),
           "a hidden tab takes no room");

    TabOptions fixed;
    fixed.width_style = 2;
    fixed.fixed_width = 900.0;
    fixed.fixed_height = 255.0;
    rects = tab_layout(tabs, fixed);
    expect(rects[1].width == twips_of_pixels(60.0) &&
               rects[1].height == twips_of_pixels(17.0),
           "fixed tabs");

    TabOptions rows;
    rows.multi_row = true;
    rows.total_width = 1000.0;
    rects = tab_layout(tabs, rows);
    expect(rects[1].left == twips_of_pixels(2.0) &&
               rects[1].top == twips_of_pixels(20.0),
           "MultiRow wraps to a new row");
    rows.multi_row = false;
    rects = tab_layout(tabs, rows);
    expect(rects[1].top == twips_of_pixels(2.0), "no wrap without MultiRow");

    // Buttons.
    std::vector<ButtonSpec> buttons{{true, kButtonDefault, 0, 2},
                                    {true, kButtonSeparator, 0, 0},
                                    {true, kButtonPlaceholder, 50, 0},
                                    {true, kButtonDefault, 700, 0},
                                    {false, kButtonDefault, 0, 0}};
    auto cells = button_layout(buttons, ButtonOptions{});
    expect(cells[0].width == twips_of_pixels(36.0) &&
               cells[0].height == twips_of_pixels(38.0) &&
               cells[0].top == twips_of_pixels(2.0),
           "a default button");
    expect(cells[1].left == twips_of_pixels(36.0) &&
               cells[1].width == twips_of_pixels(8.0),
           "a separator");
    expect(cells[2].width == twips_of_pixels(8.0),
           "a placeholder has at least 8 pixels");
    expect(std::abs(cells[3].width - 700.0) < 8.0,
           "a button with its own width");
    expect(cells[4].width == 0.0, "a hidden button is empty");

    ButtonOptions wrapped;
    wrapped.total_width = 1000.0;
    cells = button_layout(buttons, wrapped);
    expect(cells[3].left == 0.0 && cells[3].top == twips_of_pixels(40.0),
           "a full toolbar wraps");
    wrapped.wrappable = false;
    cells = button_layout(buttons, wrapped);
    expect(cells[3].top == twips_of_pixels(2.0), "no wrap without Wrappable");

    ButtonOptions sized;
    sized.fixed_width = 500.0;
    sized.button_height = 400.0;
    cells = button_layout(buttons, sized);
    expect(std::abs(cells[0].width - 500.0) < 8.0 &&
               std::abs(cells[0].height - 400.0) < 8.0,
           "ButtonWidth and ButtonHeight");

    // Groups: a run of group buttons around the position.
    const std::vector<int> styles{kButtonDefault, kButtonGroup,   kButtonGroup,
                                  kButtonGroup,   kButtonDefault, kButtonGroup};
    auto group = button_group(styles, 2);
    expect(group.first == 1 && group.second == 3, "the group around a button");
    group = button_group(styles, 0);
    expect(group.first == 0 && group.second == 3,
           "a button before a group joins it, as the loop in the evaluator "
           "always did");
    const std::vector<int> plain{kButtonDefault, kButtonDefault};
    group = button_group(plain, 0);
    expect(group.first == 0 && group.second == 0, "a button alone");
    group = button_group(styles, 5);
    expect(group.first == 5 && group.second == 5, "a group of one at the end");

    if (failures == 0) {
        std::puts("layout: ok");
    }
    return failures == 0 ? 0 : 1;
}
