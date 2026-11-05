// The input of the user for the controls of the library when a COM client
// creates them (ADR-0010, step 7): what a click, a key or the end of a label
// edit does to a TabStrip, a StatusBar, a Toolbar, a TreeView, a ListView or an
// ImageCombo, and which events it raises. The words are those of the WFC
// input scripts (`click`, `select`, `button`, `key`, `edit`), which the
// evaluator handles with the same rules from the library's headers (tree.hpp,
// list.hpp, toolbar.hpp, labeledit.hpp, layout.hpp). A window host forwards
// what the user did through WfcInput(Action, Argument) (class_factory.hpp).
// Windows only; internal to the WFC libraries.

#ifndef WFC_MSCOMCTL_COM_INPUT_HPP
#define WFC_MSCOMCTL_COM_INPUT_HPP

#include "com_collections.hpp"

#include <wfc/mscomctl/images.hpp>
#include <wfc/mscomctl/labeledit.hpp>
#include <wfc/mscomctl/layout.hpp>
#include <wfc/mscomctl/list.hpp>
#include <wfc/mscomctl/toolbar.hpp>
#include <wfc/mscomctl/tree.hpp>

#include <cctype>
#include <cstdlib>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace wfc::mscomctl::com {

using StatePtr = std::shared_ptr<ListState>;
using ItemPtr = std::shared_ptr<ItemData>;

namespace input {

// A number names an item by index, anything else by key.
[[nodiscard]] inline ItemPtr pick(const StatePtr& state,
                                  const std::string& text) {
    char* end = nullptr;
    const double number = std::strtod(text.c_str(), &end);
    const Selector selector = end != text.c_str() && *end == '\0'
                                  ? Selector::index(number)
                                  : Selector::key(text);
    const auto found = find(
        state->items.size(), selector,
        [&](const std::size_t i) { return detail::key_of(*state->items[i]); });
    return found.status == Status::ok ? state->items[found.position] : nullptr;
}

inline void set_flag(ItemData& item, const char* name, const bool on) {
    VariantClear(&item.properties[name]);
    item.properties[name].vt = VT_BOOL;
    item.properties[name].boolVal = on ? VARIANT_TRUE : VARIANT_FALSE;
}

inline void set_text(ItemData& item, const std::string& text) {
    const std::wstring wide(text.begin(), text.end());
    VariantClear(&item.properties["text"]);
    item.properties["text"].vt = VT_BSTR;
    item.properties["text"].bstrVal = SysAllocString(wide.c_str());
}

[[nodiscard]] inline int number_of(const ItemData& item, const char* name) {
    const auto found = item.properties.find(name);
    long value = 0;
    return found != item.properties.end() &&
                   detail::to_long(found->second, &value)
               ? static_cast<int>(value)
               : 0;
}

// Raises `name` with one item argument.
inline void raise_item(const StatePtr& state, const char* name,
                       const StatePtr& items, const ItemPtr& item) {
    std::vector<VARIANT> arguments{item_variant(items, item)};
    state->emit_with(name, arguments);
    VariantClear(&arguments[0]);
}

// BeforeLabelEdit(Cancel): true when the edit started.
[[nodiscard]] inline bool begin_label_edit(const StatePtr& state) {
    if (state->emit("BeforeLabelEdit", true)) {
        return false;
    }
    state->editing = true;
    return true;
}

// AfterLabelEdit(Cancel, NewString) with the text, or an empty string when the
// user cancelled; the text to keep, if any.
[[nodiscard]] inline std::optional<std::string> end_label_edit(
    const StatePtr& state, const std::optional<std::string>& text) {
    state->editing = false;
    SHORT cancel = 0;
    const std::string initial = text.value_or(std::string{});
    BSTR edited =
        SysAllocString(std::wstring(initial.begin(), initial.end()).c_str());
    std::vector<VARIANT> arguments(2);
    VariantInit(&arguments[0]);
    arguments[0].vt = VT_I2 | VT_BYREF;
    arguments[0].piVal = &cancel;
    VariantInit(&arguments[1]);
    arguments[1].vt = VT_BSTR | VT_BYREF;
    arguments[1].pbstrVal = &edited;
    state->emit_with("AfterLabelEdit", arguments);
    std::optional<std::string> accepted;
    if (keeps_edited_text(text.has_value(), cancel != 0)) {
        accepted = detail::narrow(edited);
    }
    SysFreeString(edited);
    return accepted;
}

// The edit of the item chosen by the script (`edit <text>`, `canceledit`):
// a scripted edit that was not started starts here unless the control is
// manual (labeledit.hpp).
inline void finish_edit(const StatePtr& state, const ItemPtr& item,
                        const std::optional<std::string>& text) {
    if (item == nullptr) {
        return;
    }
    if (!state->editing) {
        if (!scripted_edit_may_start(false, state->label_edit) ||
            !begin_label_edit(state)) {
            return;
        }
    }
    if (const auto accepted = end_label_edit(state, text)) {
        set_text(*item, *accepted);
    }
}

// ----- Tabs, panels, buttons and the combo

inline void tab_input(const StatePtr& tabs, const std::string& action,
                      const std::string& rest) {
    if (action == "select") {
        if (const ItemPtr tab = pick(tabs, rest)) {
            select_item_notify(*tabs, tab);
        }
    }
}

inline void panel_input(const StatePtr& panels, const std::string& action,
                        const std::string& rest) {
    if (action != "panel" && action != "paneldbl") {
        return;
    }
    if (const ItemPtr panel = pick(panels, rest)) {
        raise_item(panels, "PanelClick", panels, panel);
        if (action == "paneldbl") {
            raise_item(panels, "PanelDblClick", panels, panel);
        }
    }
}

// A press of a button group releases the others of the group.
inline void press_group(const StatePtr& buttons, const ItemPtr& button) {
    std::vector<int> styles;
    std::size_t at = 0;
    for (std::size_t i = 0; i < buttons->items.size(); ++i) {
        styles.push_back(number_of(*buttons->items[i], "style"));
        if (buttons->items[i] == button) {
            at = i;
        }
    }
    const auto [first, last] = button_group(styles, at);
    for (std::size_t i = first; i <= last; ++i) {
        ItemData& other = *buttons->items[i];
        VariantClear(&other.properties["value"]);
        other.properties["value"].vt = VT_I2;
        other.properties["value"].iVal = buttons->items[i] == button ? 1 : 0;
    }
}

inline void button_input(const StatePtr& buttons, const std::string& action,
                         const std::string& rest) {
    if (action == "button" || action == "dropdown") {
        const ItemPtr button = pick(buttons, rest);
        if (button == nullptr) {
            return;
        }
        const int style = number_of(*button, "style");
        const bool enabled = detail::flag_of(*button, "enabled");
        if (action == "dropdown") {
            if (drops_down(style, enabled)) {
                raise_item(buttons, "ButtonDropDown", buttons, button);
            }
            return;
        }
        const Press press = press_of(style, enabled);
        if (press == Press::ignore) {
            return;
        }
        if (press == Press::toggle) {
            VariantClear(&button->properties["value"]);
            button->properties["value"].vt = VT_I2;
            button->properties["value"].iVal =
                static_cast<SHORT>(toggled_value(number_of(*button, "value")));
        } else if (press == Press::group) {
            press_group(buttons, button);
        }
        raise_item(buttons, "ButtonClick", buttons, button);
        return;
    }
    if (action == "menu") {
        std::istringstream words(rest);
        std::string button_text;
        std::string menu_text;
        words >> button_text >> menu_text;
        const ItemPtr button = pick(buttons, button_text);
        if (button == nullptr || button->sub == nullptr) {
            return;
        }
        const ItemPtr menu = pick(button->sub, menu_text);
        if (menu != nullptr && detail::flag_of(*menu, "enabled")) {
            raise_item(buttons, "ButtonMenuClick", button->sub, menu);
        }
        return;
    }
    if (action == "dblclick") {
        // With no customization dialog only the events run.
        buttons->emit("DblClick");
        if (buttons->allow_customize) {
            buttons->emit("Change");
        }
        return;
    }
    if (action == "customize") {
        buttons->emit("Change");
    }
}

inline void combo_input(const StatePtr& combo, const std::string& action,
                        const std::string& rest) {
    if (action == "select") {
        if (const ItemPtr item = pick(combo, rest)) {
            select_item_notify(*combo, item);
        }
    } else if (action == "dropdown") {
        combo->emit("Dropdown");
    }
}

// ----- TreeView

// Expands or collapses `node`, raising the event when the state changes.
inline void set_expanded(const StatePtr& tree, const ItemPtr& node,
                         const bool expand) {
    if (detail::flag_of(*node, "expanded") == expand) {
        return;
    }
    set_flag(*node, "expanded", expand);
    raise_item(tree, expand ? "Expand" : "Collapse", tree, node);
}

// The nodes in display order: the kids of a collapsed node are left out.
inline void visible_rows(const StatePtr& tree,
                         const std::vector<ItemData*>& kids, const int depth,
                         std::vector<ItemPtr>& nodes, std::vector<Row>& rows) {
    for (ItemData* kid : kids) {
        const ItemPtr node = tree->share(kid);
        if (node == nullptr) {
            continue;
        }
        const std::string text = detail::text_of(*node, "text");
        Row row;
        row.depth = depth;
        row.text_length = text.size();
        row.first = text.empty() ? '\0'
                                 : static_cast<char>(std::tolower(
                                       static_cast<unsigned char>(text[0])));
        row.has_children = !node->kids.empty();
        row.expanded = detail::flag_of(*node, "expanded");
        nodes.push_back(node);
        rows.push_back(row);
        if (row.expanded) {
            visible_rows(tree, node->kids, depth + 1, nodes, rows);
        }
    }
}

inline void tree_input(const StatePtr& tree, const std::string& action,
                       const std::string& rest) {
    if (action == "click") {
        const ItemPtr node = pick(tree, rest);
        if (node == nullptr) {
            return;
        }
        const ItemPtr previous = tree->selected;
        const bool second_click = previous == node;
        tree->selected = node;
        if (tree->single_sel) {
            bool ancestor = false;
            for (const ItemData* at = node->parent; at != nullptr;
                 at = at->parent) {
                ancestor = ancestor || at == previous.get();
            }
            if (collapses_previous(previous != nullptr, previous == node,
                                   ancestor)) {
                set_expanded(tree, previous, false);
            }
            set_expanded(tree, node, true);
        }
        raise_item(tree, "NodeClick", tree, node);
        tree->emit("Click");
        if (click_starts_edit(second_click, tree->label_edit)) {
            static_cast<void>(begin_label_edit(tree));
        }
        return;
    }
    if (action == "expand" || action == "collapse") {
        if (const ItemPtr node = pick(tree, rest)) {
            set_expanded(tree, node, action == "expand");
        }
        return;
    }
    if (action == "check") {
        const ItemPtr node = pick(tree, rest);
        if (node != nullptr && tree->checkboxes) {
            set_flag(*node, "checked", !detail::flag_of(*node, "checked"));
            raise_item(tree, "NodeCheck", tree, node);
        }
        return;
    }
    if (action == "edit" || action == "canceledit") {
        // The end of an edit of the selected node.
        finish_edit(
            tree, tree->selected,
            action == "edit" ? std::optional<std::string>(rest) : std::nullopt);
        return;
    }
    if (action == "key") {
        std::vector<ItemPtr> nodes;
        std::vector<Row> rows;
        visible_rows(tree, tree->roots, 0, nodes, rows);
        std::size_t at = rows.size();
        for (std::size_t i = 0; i < nodes.size(); ++i) {
            if (nodes[i] == tree->selected) {
                at = i;
            }
        }
        std::string key = rest;
        for (char& c : key) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        const auto effect = key_effect(rows, at, key, tree->checkboxes);
        const ItemPtr current = tree->selected;
        using Kind = KeyEffect::Kind;
        if (current == nullptr && effect.kind != Kind::select) {
            return;
        }
        switch (effect.kind) {
            case Kind::select:
                tree->selected = nodes[effect.row];
                break;
            case Kind::expand:
                set_expanded(tree, current, true);
                break;
            case Kind::collapse:
                set_expanded(tree, current, false);
                break;
            case Kind::toggle:
                set_flag(*current, "checked",
                         !detail::flag_of(*current, "checked"));
                raise_item(tree, "NodeCheck", tree, current);
                break;
            case Kind::none:
                break;
        }
    }
}

// ----- ListView

inline void list_input(const StatePtr& list, const StatePtr& columns,
                       const std::string& action, const std::string& rest) {
    if (action == "click" || action == "dblclick") {
        std::istringstream words(rest);
        std::string name;
        std::string modifier;
        words >> name >> modifier;
        const ItemPtr item = pick(list, name);
        if (item == nullptr) {
            return;
        }
        const bool ctrl = modifier == "ctrl";
        const bool second_click =
            is_second_click(detail::flag_of(*item, "selected"), ctrl);
        std::vector<bool> chosen;
        std::size_t position = 0;
        for (std::size_t i = 0; i < list->items.size(); ++i) {
            chosen.push_back(detail::flag_of(*list->items[i], "selected"));
            if (list->items[i] == item) {
                position = i;
            }
        }
        select_item(chosen, position, click_extends(ctrl, list->multi_select));
        for (std::size_t i = 0; i < chosen.size(); ++i) {
            set_flag(*list->items[i], "selected", chosen[i]);
        }
        raise_item(list, "ItemClick", list, item);
        list->emit("Click");
        if (action == "dblclick") {
            list->emit("DblClick");
        }
        if (action == "click" &&
            click_starts_edit(second_click, list->label_edit)) {
            static_cast<void>(begin_label_edit(list));
        }
        return;
    }
    if (action == "check") {
        const ItemPtr item = pick(list, rest);
        if (item != nullptr && list->checkboxes) {
            set_flag(*item, "checked", !detail::flag_of(*item, "checked"));
            raise_item(list, "ItemCheck", list, item);
        }
        return;
    }
    if (action == "columnclick") {
        if (const ItemPtr column = pick(columns, rest)) {
            raise_item(list, "ColumnClick", columns, column);
        }
        return;
    }
    if (action == "edit" || action == "canceledit") {
        ItemPtr first;
        for (const auto& item : list->items) {
            if (detail::flag_of(*item, "selected")) {
                first = item;
                break;
            }
        }
        finish_edit(
            list, first,
            action == "edit" ? std::optional<std::string>(rest) : std::nullopt);
    }
}

}  // namespace input
}  // namespace wfc::mscomctl::com

#endif  // WFC_MSCOMCTL_COM_INPUT_HPP
