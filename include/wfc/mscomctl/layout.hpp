// The layout of the tabs of a TabStrip and of the buttons of a Toolbar, apart
// from any window or control: sizes in twips from the properties of the items,
// and the rule for which buttons of a toolbar form one group. Header only, with
// no dependency on the WFC evaluator or on Windows; the evaluator and the tests
// use it. Internal to the WFC libraries.
//
// The button layout follows what Visual Basic 6 reports for a Toolbar under
// the classic common controls (evidence/reference/control-render): a button
// is 38 pixels high and at least 36 wide, the first row starts 2 pixels below
// the top, and a size in twips is the whole himetric of the pixels (so 36
// pixels are 540.2835 twips). The tab layout is still an approximation.

#ifndef WFC_MSCOMCTL_LAYOUT_HPP
#define WFC_MSCOMCTL_LAYOUT_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace wfc::mscomctl {

// The position and size of a tab or a button, in twips.
struct Rect {
    double left{0};
    double top{0};
    double width{0};
    double height{0};
};

// What the layout of a tab needs to know of it.
struct TabSpec {
    bool visible = true;
    std::size_t caption_length = 0;
    double text_width = -1;  // in pixels, -1 to estimate from the length
};

// The properties of a TabStrip that decide the layout.
struct TabOptions {
    double fixed_width = 0;   // TabFixedWidth
    double fixed_height = 0;  // TabFixedHeight
    double min_width = 0;     // TabMinWidth
    long width_style = 0;     // TabWidthStyle: 2 is tabFixed
    double total_width = 0;   // Width of the control
    bool multi_row = false;   // MultiRow
    double left = 0;          // Left of the control
    double top = 0;           // Top of the control
};

// The twips Visual Basic 6 reports for a size of `pixels` at 96 dpi: the
// pixels in whole himetric, then in twips.
[[nodiscard]] inline double twips_of_pixels(const double pixels) {
    const double himetric =
        static_cast<double>(static_cast<long>(pixels * 2540.0 / 96.0 + 0.5));
    // Single precision, as Visual Basic computes it.
    return static_cast<double>(static_cast<float>(himetric) *
                               (1440.0F / 2540.0F));
}

// The width in pixels of `text` in the default font of the controls (MS Sans
// Serif 8 point), from a table fitted to what Visual Basic 6 measures.
[[nodiscard]] inline double text_pixels(const std::string& text) {
    double total = 0.0;
    for (const char c : text) {
        if (std::string_view{"il.,:;!'|"}.find(c) != std::string_view::npos) {
            total += 2.0;
        } else if (std::string_view{"tfr I()[]"}.find(c) !=
                   std::string_view::npos) {
            total += 3.0;
        } else if (std::string_view{"sczkvxyJF"}.find(c) !=
                   std::string_view::npos) {
            total += 5.0;
        } else if (std::string_view{"mwMW"}.find(c) != std::string_view::npos) {
            total += 9.0;
        } else if (c >= 'A' && c <= 'Z' && c != 'C') {
            total += 7.0;
        } else {
            total += 6.0;
        }
    }
    return total;
}

// The rectangle of every tab, laid out left to right (wrapping to rows with
// MultiRow), in the coordinates of the container: a tab is 18 pixels high and
// the first starts 2 pixels inside the control. A tab is as wide as its text
// (about 5 pixels a character) plus 13 pixels. A tab that is not visible has
// an empty rectangle.
[[nodiscard]] inline std::vector<Rect> tab_layout(
    const std::vector<TabSpec>& tabs, const TabOptions& options) {
    constexpr double kTwipsPerPixel = 15.0;
    std::vector<Rect> rects;
    const double height = options.fixed_height > 0.0
                              ? options.fixed_height / kTwipsPerPixel
                              : 18.0;
    const double origin_left = std::floor(options.left / kTwipsPerPixel) + 2.0;
    const double origin_top = std::floor(options.top / kTwipsPerPixel) + 2.0;
    double x = 0.0;
    double y = 0.0;
    for (const auto& tab : tabs) {
        if (!tab.visible) {
            rects.push_back({});
            continue;
        }
        const double width =
            options.width_style == 2 && options.fixed_width > 0.0
                ? options.fixed_width / kTwipsPerPixel
                : std::max(options.min_width / kTwipsPerPixel,
                           13.0 + (tab.text_width >= 0.0
                                       ? tab.text_width
                                       : 5.0 * static_cast<double>(
                                                   tab.caption_length)));
        if (options.multi_row && x > 0.0 &&
            (x + width) * kTwipsPerPixel > options.total_width &&
            options.total_width > 0.0) {
            x = 0.0;
            y += height;
        }
        rects.push_back({twips_of_pixels(origin_left + x),
                         twips_of_pixels(origin_top + y),
                         twips_of_pixels(width), twips_of_pixels(height)});
        x += width;
    }
    return rects;
}

// The styles of a toolbar button (tbrStyleConstants).
constexpr int kButtonDefault = 0;
constexpr int kButtonCheck = 1;
constexpr int kButtonGroup = 2;
constexpr int kButtonSeparator = 3;
constexpr int kButtonPlaceholder = 4;
constexpr int kButtonDropdown = 5;

// What the layout of a button needs to know of it.
struct ButtonSpec {
    bool visible = true;
    int style = kButtonDefault;
    double width = 0;  // the button's own Width, 0 for the default
    std::size_t caption_length = 0;
};

// The properties of a Toolbar that decide the layout.
struct ButtonOptions {
    double button_height = 0;  // ButtonHeight, 0 for the default
    double fixed_width = 0;    // ButtonWidth, 0 for the default
    double total_width = 0;    // Width of the control
    bool wrappable = true;     // Wrappable
};

// Position and size of every button (wrapping with Wrappable starts a new row
// when the toolbar is full). A button that is not visible has an empty
// rectangle. The positions add up in pixels and are reported in the twips of
// whole pixels, as Visual Basic 6 does.
[[nodiscard]] inline std::vector<Rect> button_layout(
    const std::vector<ButtonSpec>& buttons, const ButtonOptions& options) {
    constexpr double kTwipsPerPixel = 15.0;
    const auto pixels = [](const double twips) {
        return twips / kTwipsPerPixel;
    };
    std::vector<Rect> rects;
    const double height =
        options.button_height >= 1.0 ? pixels(options.button_height) : 38.0;
    double x = 0.0;
    double y = 2.0;
    for (const auto& button : buttons) {
        if (!button.visible) {
            rects.push_back({});
            continue;
        }
        double width = 36.0;
        if (button.style == kButtonSeparator) {
            width = 8.0;
        } else if (button.style == kButtonPlaceholder) {
            width = std::max(pixels(button.width), 8.0);
        } else if (button.width > 0.0) {
            width = pixels(button.width);
        } else if (options.fixed_width >= 1.0) {
            width = pixels(options.fixed_width);
        } else {
            width = std::max(
                36.0, 6.0 + 6.0 * static_cast<double>(button.caption_length));
        }
        if (options.wrappable && options.total_width > 0.0 && x > 0.0 &&
            (x + width) * kTwipsPerPixel > options.total_width) {
            x = 0.0;
            y += height;
        }
        rects.push_back({twips_of_pixels(x), twips_of_pixels(y),
                         twips_of_pixels(width), twips_of_pixels(height)});
        x += width;
    }
    return rects;
}

// The buttons that form a group with the button at `position` (0-based): the
// run of consecutive buttons of style group around it, as the first and last
// positions (the button itself when it is alone).
[[nodiscard]] inline std::pair<std::size_t, std::size_t> button_group(
    const std::vector<int>& styles, const std::size_t position) {
    std::size_t first = position;
    std::size_t last = position;
    while (first > 0 && styles[first - 1U] == kButtonGroup) {
        --first;
    }
    while (last + 1U < styles.size() && styles[last + 1U] == kButtonGroup) {
        ++last;
    }
    return {first, last};
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_LAYOUT_HPP
