// The rules of an ImageList and of an ImageCombo, apart from any window,
// control or evaluator: what removing an image of a bound list does, the size
// the first image gives the list, Overlay, and the matching and selection of
// the text of a combo. Header only, with no dependency on the WFC evaluator or
// on Windows; the evaluator and the tests use it. Internal to the WFC
// libraries.

#ifndef WFC_MSCOMCTL_IMAGES_HPP
#define WFC_MSCOMCTL_IMAGES_HPP

#include <wfc/mscomctl/items.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wfc::mscomctl {

// Remove and Clear on a ListImages collection whose list is bound to a control.
[[nodiscard]] constexpr Status check_image_removal(const bool bound) {
    return bound ? Status::image_list_remove_bound : Status::ok;
}

// ImageWidth and ImageHeight can be set only while the list holds no image.
[[nodiscard]] constexpr bool image_size_settable(const std::size_t count) {
    return count == 0U;
}

// The first image added gives the list its size.
[[nodiscard]] constexpr bool first_image_sets_size(const std::size_t count) {
    return count == 1U;
}

// A picture measures himetric (0.01 mm); the list measures pixels at 96 dpi.
[[nodiscard]] inline short image_pixels(const long himetric) {
    return static_cast<short>(
        std::lround(static_cast<double>(himetric) * 96.0 / 2540.0));
}

// Overlay(Index1, Index2) takes exactly two images.
[[nodiscard]] constexpr bool overlay_arguments_ok(const std::size_t count) {
    return count == 2U;
}

// The text of the combo and the text of an item are the same without regard
// to case (ASCII), including two empty texts.
[[nodiscard]] inline bool same_text(const std::string_view left,
                                    const std::string_view right) {
    return !text_before(left, right) && !text_before(right, left);
}

// Setting the Text of an ImageCombo selects the first item that has exactly
// that text; none otherwise.
[[nodiscard]] inline std::optional<std::size_t> item_with_text(
    const std::vector<std::string>& texts, const std::string_view text) {
    for (std::size_t i = 0; i < texts.size(); ++i) {
        if (same_text(texts[i], text)) {
            return i;
        }
    }
    return std::nullopt;
}

// The selected part of the text, from SelStart and SelLength (which may lie
// beyond the text or be negative, as they are stored unchecked).
struct TextRange {
    std::size_t start = 0;
    std::size_t length = 0;
};

[[nodiscard]] inline TextRange selected_range(const std::size_t text_size,
                                              const double sel_start,
                                              const double sel_length) {
    TextRange range;
    range.start =
        std::min(static_cast<std::size_t>(std::max(0.0, sel_start)), text_size);
    range.length = std::min(static_cast<std::size_t>(std::max(0.0, sel_length)),
                            text_size - range.start);
    return range;
}

[[nodiscard]] inline std::string selected_text(const std::string_view text,
                                               const double sel_start,
                                               const double sel_length) {
    const auto range = selected_range(text.size(), sel_start, sel_length);
    return std::string(text.substr(range.start, range.length));
}

// SelText = inserted: replaces the selected part.
[[nodiscard]] inline std::string replace_selected(
    const std::string_view text, const double sel_start,
    const double sel_length, const std::string_view inserted) {
    const auto range = selected_range(text.size(), sel_start, sel_length);
    std::string result(text.substr(0, range.start));
    result += inserted;
    result += text.substr(range.start + range.length);
    return result;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_IMAGES_HPP
