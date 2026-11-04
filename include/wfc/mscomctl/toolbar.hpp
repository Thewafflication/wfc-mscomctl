// The rules of the buttons of a Toolbar, apart from any window, control or
// evaluator: which Style and Value are valid, what pressing a button does,
// when a drop-down or a menu entry answers, and the text SaveToolbar keeps.
// Header only, with no dependency on the WFC evaluator or on Windows; the
// evaluator and the tests use it. Internal to the WFC libraries.
//
// The positions and sizes of the buttons, and which buttons form a group, are
// in layout.hpp.

#ifndef WFC_MSCOMCTL_TOOLBAR_HPP
#define WFC_MSCOMCTL_TOOLBAR_HPP

#include <wfc/mscomctl/layout.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace wfc::mscomctl {

// Button.Style runs from tbrDefault (0) to tbrDropdown (5).
[[nodiscard]] constexpr bool valid_button_style(const double style) {
    return style >= 0.0 && style <= 5.0;
}

// Button.Value is tbrUnpressed (0) or tbrPressed (1).
[[nodiscard]] constexpr bool valid_button_value(const double value) {
    return value >= 0.0 && value <= 1.0;
}

// What a press of a button does before ButtonClick is raised.
enum class Press {
    ignore,  // disabled, separator or placeholder: nothing happens
    plain,   // ButtonClick only
    toggle,  // a check button changes its Value, then ButtonClick
    group,   // a group button is pressed and its neighbours released
};

[[nodiscard]] constexpr Press press_of(const int style, const bool enabled) {
    if (!enabled || style == kButtonSeparator || style == kButtonPlaceholder) {
        return Press::ignore;
    }
    if (style == kButtonCheck) {
        return Press::toggle;
    }
    if (style == kButtonGroup) {
        return Press::group;
    }
    return Press::plain;
}

// The Value of a check button after a press.
[[nodiscard]] constexpr int toggled_value(const double value) {
    return value == 0.0 ? 1 : 0;
}

// Setting Value to tbrPressed on a group button presses it as a click does.
[[nodiscard]] constexpr bool value_presses_group(const int style,
                                                 const double value) {
    return value == 1.0 && style == kButtonGroup;
}

// A drop-down button raises ButtonDropDown only when it is enabled.
[[nodiscard]] constexpr bool drops_down(const int style, const bool enabled) {
    return style == kButtonDropdown && enabled;
}

// What SaveToolbar keeps of a button: its Key and whether it is visible.
struct SavedButton {
    std::string key;
    bool visible = true;
};

// "key=1;key=0;" in button order.
[[nodiscard]] inline std::string save_layout(
    const std::vector<SavedButton>& buttons) {
    std::string text;
    for (const auto& button : buttons) {
        text += button.key + "=" + (button.visible ? "1" : "0") + ";";
    }
    return text;
}

// The entries of a saved layout. A piece without "=" is skipped.
[[nodiscard]] inline std::vector<SavedButton> parse_layout(
    const std::string_view text) {
    std::vector<SavedButton> entries;
    std::size_t start = 0;
    while (start < text.size()) {
        const auto end = text.find(';', start);
        const auto entry = text.substr(start, end - start);
        const auto equals = entry.find('=');
        if (equals != std::string_view::npos) {
            entries.push_back({std::string(entry.substr(0, equals)),
                               entry.substr(equals + 1U) == "1"});
        }
        if (end == std::string_view::npos) {
            break;
        }
        start = end + 1U;
    }
    return entries;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_TOOLBAR_HPP
