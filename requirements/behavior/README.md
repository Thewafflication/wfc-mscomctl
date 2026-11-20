# MSComctlLib Behavioral Requirements

**Content type:** Project requirement index  
**Status:** Proposed  
**Reference documentation:** `C:\Windows\Help\cmctl198.chm`  
**Reference documentation SHA-256:** `B60543DFA968658287DAF3939B44B35C2243D0252FEF7A7D4C6F5CCF13BF92D6`

These requirements translate the installed Microsoft Common Controls 6.0 help
into observable WFC obligations. They supplement the Automation API contracts
in the parent directory. Each remains proposed until its VB6 reference test
confirms the documented behavior against the recorded binary baseline.

## TabStrip

- [REQ-0017 — Collection and selection](req-0017-tabstrip-collection-selection.md)
- [REQ-0018 — Layout behavior](req-0018-tabstrip-layout.md)
- [REQ-0019 — Pre-selection cancellation](req-0019-tabstrip-before-click.md)

## Toolbar

- [REQ-0020 — Button behavior](req-0020-toolbar-buttons.md)
- [REQ-0021 — Customization](req-0021-toolbar-customization.md)
- [REQ-0022 — State persistence](req-0022-toolbar-persistence.md)

## StatusBar

- [REQ-0023 — Panel model](req-0023-statusbar-panels.md)
- [REQ-0024 — Normal and simple modes](req-0024-statusbar-modes.md)

## ProgressBar

- [REQ-0025 — Range and rendering](req-0025-progressbar-range.md)
- [REQ-0026 — Orientation and fill style](req-0026-progressbar-orientation-scrolling.md)

## TreeView

- [REQ-0027 — Hierarchy management](req-0027-treeview-hierarchy.md)
- [REQ-0028 — Keyboard navigation](req-0028-treeview-keyboard.md)
- [REQ-0029 — Label editing](req-0029-treeview-label-edit.md)
- [REQ-0030 — Visibility and hit testing](req-0030-treeview-spatial-query.md)

## ListView

- [REQ-0031 — View modes](req-0031-listview-views.md)
- [REQ-0032 — Item and column model](req-0032-listview-object-model.md)
- [REQ-0033 — Searching and sorting](req-0033-listview-search-sort.md)
- [REQ-0034 — Label editing](req-0034-listview-edit-presentation.md)
- [REQ-0041 — Report presentation options](req-0041-listview-report-presentation.md)

## ImageList

- [REQ-0035 — Collection and binding](req-0035-imagelist-collection-binding.md)
- [REQ-0036 — Rendering and composition](req-0036-imagelist-render-composition.md)

## Slider

- [REQ-0037 — Value and selection range](req-0037-slider-range-selection.md)
- [REQ-0038 — Input and event timing](req-0038-slider-input-events.md)
- [REQ-0039 — Ticks and orientation](req-0039-slider-ticks-orientation.md)

## ImageCombo

- [REQ-0040 — Collection insertion](req-0040-imagecombo-items.md)
- [REQ-0042 — Item presentation](req-0042-imagecombo-item-presentation.md)
- [REQ-0043 — Editing lock](req-0043-imagecombo-locking.md)

## Evidence and Acceptance

The help file is a specification source, not execution evidence. A requirement
becomes eligible for acceptance only after a VB6 reference probe records the
installed control's actual behavior, resolves any difference from the help,
and verifies WFC against the resulting expected outcome. Meanwhile each
requirement names the test cases of this library that check WFC's behavior
(`TC-NNNN`, `tests/tc-NNNN-*.tex`).
