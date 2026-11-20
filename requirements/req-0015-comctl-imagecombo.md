# REQ-0015 — ImageCombo behavior in WFC

**Content type:** Project requirement

**Status:** Accepted (implements `REQ-0040`, `REQ-0042` and `REQ-0043` without reference probes)

## Statement

- **Items.** `ComboItems.Add(Index, Key, Text, Image, SelImage, Indentation)`;
  `ComboItem` has `Text` (the default), `Key`, `Tag`, `Image`, `SelImage`,
  `Indentation`, `Selected` and `Index`. `GetFirstVisible` returns the first item
  or `Nothing`.
- **Selection and text.** `SelectedItem` returns the selected item or `Nothing`;
  assigning an item (or setting `ComboItem.Selected = True`) selects it, copies its
  text to `Text` and raises `Click` then `Change`; assigning `Nothing` clears the
  selection. Setting `Text` raises `Change` when it changes and selects the item
  whose text matches (ignoring case), else none. Removing the selected item clears
  the selection and keeps the text.
- **Selection text.** `SelStart`, `SelLength` and `SelText` address a part of
  `Text`; assigning `SelText` replaces that part, sets `SelLength` to the new
  length and raises `Change`.
- **Locking.** With `Locked = True` the user cannot edit the text or change the
  selection by typing; code is unaffected.
- **User actions (scripted).** `ui <combo> select <item>` selects (`Click`,
  `Change`), `type <text>` edits (ignored when locked), `dropdown` raises
  `Dropdown`.

## Verification

**Method:** Test

**References:** `TC-0008`, `TC-0015`, `TC-0022`

The corpus program that exercises it and the unit tests of the rules it uses are listed above; each test case specification states what is checked.

