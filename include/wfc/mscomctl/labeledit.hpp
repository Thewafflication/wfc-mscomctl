// Label editing of a TreeView or a ListView, apart from the control: when an
// edit starts and which text it keeps. Header only, with no dependency on the
// WFC evaluator or on Windows. Internal to the WFC libraries.

#ifndef WFC_MSCOMCTL_LABELEDIT_HPP
#define WFC_MSCOMCTL_LABELEDIT_HPP

namespace wfc::mscomctl {

// LabelEdit: 0 is lvwAutomatic, 1 is lvwManual. A second click on the selected
// node starts an edit only when it is automatic.
[[nodiscard]] inline bool click_starts_edit(const bool second_click,
                                            const double label_edit) {
    return second_click && label_edit != 1.0;
}

// The end of an edit keeps the text only if there was one (the user did not
// press Escape) and AfterLabelEdit left Cancel at zero.
[[nodiscard]] constexpr bool keeps_edited_text(const bool has_text,
                                               const bool cancelled) {
    return has_text && !cancelled;
}

// A scripted edit (the end of an edit that was not started) first starts one
// unless the control is manual.
[[nodiscard]] constexpr bool scripted_edit_may_start(const bool editing,
                                                     const double label_edit) {
    return editing || label_edit != 1.0;
}

}  // namespace wfc::mscomctl

#endif  // WFC_MSCOMCTL_LABELEDIT_HPP
