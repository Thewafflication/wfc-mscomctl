# Identifier map

**Content type:** Project record

The requirements and test cases of this library were first numbered in the
[WFC](https://github.com/Thewafflication/wfc) repository's sequence (`REQ-0001`
to `REQ-0036`, `REQ-0296` to `REQ-0302`; CTest names such as
`TC-MP0010-mscomctl-tables`). Since the library is its own repository, it
numbers `REQ-NNNN` and `TC-NNNN` from 0001 in its own namespace
(`wsp/style/artifact-identifiers.md`). This file keeps the old identifiers so
that references to them stay resolvable; a document that cites an old
identifier means the library artifact named here.

The new numbers are stable: do not renumber or reuse them.

## Requirements

| Old ID | New ID | Title | File |
| --- | --- | --- | --- |
| `REQ-0001` | `REQ-0001` | TabStrip Automation API contract | `requirements/req-0001-tabstrip-api-contract.md` |
| `REQ-0002` | `REQ-0002` | Toolbar Automation API contract | `requirements/req-0002-toolbar-api-contract.md` |
| `REQ-0003` | `REQ-0003` | StatusBar Automation API contract | `requirements/req-0003-statusbar-api-contract.md` |
| `REQ-0004` | `REQ-0004` | ProgressBar Automation API contract | `requirements/req-0004-progressbar-api-contract.md` |
| `REQ-0005` | `REQ-0005` | TreeView Automation API contract | `requirements/req-0005-treeview-api-contract.md` |
| `REQ-0006` | `REQ-0006` | ListView Automation API contract | `requirements/req-0006-listview-api-contract.md` |
| `REQ-0007` | `REQ-0007` | ImageList Automation API contract | `requirements/req-0007-imagelist-api-contract.md` |
| `REQ-0008` | `REQ-0008` | Slider Automation API contract | `requirements/req-0008-slider-api-contract.md` |
| `REQ-0009` | `REQ-0009` | ImageCombo Automation API contract | `requirements/req-0009-imagecombo-api-contract.md` |
| `REQ-0296` | `REQ-0010` | MSComctlLib collections, value objects and the native controls | `requirements/req-0010-comctl-object-model.md` |
| `REQ-0297` | `REQ-0011` | TabStrip and StatusBar behavior in WFC | `requirements/req-0011-comctl-tabstrip-statusbar.md` |
| `REQ-0298` | `REQ-0012` | Toolbar behavior in WFC | `requirements/req-0012-comctl-toolbar.md` |
| `REQ-0299` | `REQ-0013` | TreeView behavior in WFC | `requirements/req-0013-comctl-treeview.md` |
| `REQ-0300` | `REQ-0014` | ListView behavior in WFC | `requirements/req-0014-comctl-listview.md` |
| `REQ-0301` | `REQ-0015` | ImageCombo behavior in WFC | `requirements/req-0015-comctl-imagecombo.md` |
| `REQ-0302` | `REQ-0016` | Persisted MSComctlLib state in form files | `requirements/req-0016-comctl-persistence.md` |
| `REQ-0010` | `REQ-0017` | TabStrip collection and selection | `requirements/behavior/req-0017-tabstrip-collection-selection.md` |
| `REQ-0011` | `REQ-0018` | TabStrip layout behavior | `requirements/behavior/req-0018-tabstrip-layout.md` |
| `REQ-0012` | `REQ-0019` | TabStrip pre-selection cancellation | `requirements/behavior/req-0019-tabstrip-before-click.md` |
| `REQ-0013` | `REQ-0020` | Toolbar button behavior | `requirements/behavior/req-0020-toolbar-buttons.md` |
| `REQ-0014` | `REQ-0021` | Toolbar customization | `requirements/behavior/req-0021-toolbar-customization.md` |
| `REQ-0015` | `REQ-0022` | Toolbar state persistence | `requirements/behavior/req-0022-toolbar-persistence.md` |
| `REQ-0016` | `REQ-0023` | StatusBar panel model | `requirements/behavior/req-0023-statusbar-panels.md` |
| `REQ-0017` | `REQ-0024` | StatusBar normal and simple modes | `requirements/behavior/req-0024-statusbar-modes.md` |
| `REQ-0018` | `REQ-0025` | ProgressBar range and rendering | `requirements/behavior/req-0025-progressbar-range.md` |
| `REQ-0019` | `REQ-0026` | ProgressBar orientation and fill style | `requirements/behavior/req-0026-progressbar-orientation-scrolling.md` |
| `REQ-0020` | `REQ-0027` | TreeView hierarchy management | `requirements/behavior/req-0027-treeview-hierarchy.md` |
| `REQ-0021` | `REQ-0028` | TreeView keyboard navigation | `requirements/behavior/req-0028-treeview-keyboard.md` |
| `REQ-0022` | `REQ-0029` | TreeView label editing | `requirements/behavior/req-0029-treeview-label-edit.md` |
| `REQ-0023` | `REQ-0030` | TreeView visibility and hit testing | `requirements/behavior/req-0030-treeview-spatial-query.md` |
| `REQ-0024` | `REQ-0031` | ListView view modes | `requirements/behavior/req-0031-listview-views.md` |
| `REQ-0025` | `REQ-0032` | ListView item and column model | `requirements/behavior/req-0032-listview-object-model.md` |
| `REQ-0026` | `REQ-0033` | ListView searching and sorting | `requirements/behavior/req-0033-listview-search-sort.md` |
| `REQ-0027` | `REQ-0034` | ListView label editing | `requirements/behavior/req-0034-listview-edit-presentation.md` |
| `REQ-0028` | `REQ-0035` | ImageList collection and binding | `requirements/behavior/req-0035-imagelist-collection-binding.md` |
| `REQ-0029` | `REQ-0036` | ImageList rendering and composition | `requirements/behavior/req-0036-imagelist-render-composition.md` |
| `REQ-0030` | `REQ-0037` | Slider value and selection range | `requirements/behavior/req-0037-slider-range-selection.md` |
| `REQ-0031` | `REQ-0038` | Slider input and event timing | `requirements/behavior/req-0038-slider-input-events.md` |
| `REQ-0032` | `REQ-0039` | Slider ticks and orientation | `requirements/behavior/req-0039-slider-ticks-orientation.md` |
| `REQ-0033` | `REQ-0040` | ImageCombo collection insertion | `requirements/behavior/req-0040-imagecombo-items.md` |
| `REQ-0034` | `REQ-0041` | ListView report presentation options | `requirements/behavior/req-0041-listview-report-presentation.md` |
| `REQ-0035` | `REQ-0042` | ImageCombo item presentation | `requirements/behavior/req-0042-imagecombo-item-presentation.md` |
| `REQ-0036` | `REQ-0043` | ImageCombo editing lock | `requirements/behavior/req-0043-imagecombo-locking.md` |

`REQ-0001` to `REQ-0009` keep their numbers. WFC's own requirements with the
same numbers (for example `REQ-0010` of WFC) are different artifacts; the
library requirement carries the old number in the table above only.

## Test cases

| Old ID | New ID | Title | Implementation | Specification |
| --- | --- | --- | --- | --- |
| `TC-MP0010-mscomctl-tables` | `TC-0001-tables` | Member table of the common controls | `tests/mscomctl_tables_test.cpp` | `tests/tc-0001-tables.tex` |
| `TC-MP0010-mscomctl-range` | `TC-0002-range` | Range rules of ProgressBar and Slider | `tests/range_test.cpp` | `tests/tc-0002-range.tex` |
| `TC-MP0010-mscomctl-layout` | `TC-0003-layout` | Layout of tabs and toolbar buttons | `tests/layout_test.cpp` | `tests/tc-0003-layout.tex` |
| `TC-MP0010-mscomctl-items` | `TC-0004-items` | Keys, indexes and collection errors | `tests/items_test.cpp` | `tests/tc-0004-items.tex` |
| `TC-MP0010-mscomctl-toolbar` | `TC-0005-toolbar` | Toolbar button rules | `tests/toolbar_test.cpp` | `tests/tc-0005-toolbar.tex` |
| `TC-MP0010-mscomctl-tree` | `TC-0006-tree` | TreeView rules | `tests/tree_test.cpp` | `tests/tc-0006-tree.tex` |
| `TC-MP0010-mscomctl-list` | `TC-0007-list` | ListView rules | `tests/list_test.cpp` | `tests/tc-0007-list.tex` |
| `TC-MP0010-mscomctl-images` | `TC-0008-images` | ImageList and ImageCombo rules | `tests/images_test.cpp` | `tests/tc-0008-images.tex` |
| `TC-MP0010-mscomctl-dll` | `TC-0009-dll` | Library DLL: exports, version and registration | `tests/mscomctl_dll_test.c` | `tests/tc-0009-dll.tex` |
| `TC-MP0002-corpus-188-comctl-basic` | `TC-0010-comctl-basic` | Basic controls: ProgressBar, Slider, ImageList | `tests/corpus/188-comctl-basic.vbp` (run by WFC) | `tests/tc-0010-comctl-basic.tex` |
| `TC-MP0002-corpus-189-comctl-tabstatus` | `TC-0011-comctl-tabstatus` | TabStrip and StatusBar | `tests/corpus/189-comctl-tabstatus.vbp` (run by WFC) | `tests/tc-0011-comctl-tabstatus.tex` |
| `TC-MP0002-corpus-190-comctl-toolbar` | `TC-0012-comctl-toolbar` | Toolbar buttons | `tests/corpus/190-comctl-toolbar.vbp` (run by WFC) | `tests/tc-0012-comctl-toolbar.tex` |
| `TC-MP0002-corpus-191-comctl-treeview` | `TC-0013-comctl-treeview` | TreeView nodes | `tests/corpus/191-comctl-treeview.vbp` (run by WFC) | `tests/tc-0013-comctl-treeview.tex` |
| `TC-MP0002-corpus-192-comctl-listview` | `TC-0014-comctl-listview` | ListView items and columns | `tests/corpus/192-comctl-listview.vbp` (run by WFC) | `tests/tc-0014-comctl-listview.tex` |
| `TC-MP0002-corpus-193-comctl-imagecombo` | `TC-0015-comctl-imagecombo` | ImageCombo items and text | `tests/corpus/193-comctl-imagecombo.vbp` (run by WFC) | `tests/tc-0015-comctl-imagecombo.tex` |
| `TC-MP0002-corpus-194-comctl-persisted` | `TC-0016-comctl-persisted` | Persisted state in a form file | `tests/corpus/194-comctl-persisted.vbp` (run by WFC) | `tests/tc-0016-comctl-persisted.tex` |
| `TC-MP0002-corpus-195-comctl-refine` | `TC-0017-comctl-refine` | Refinements of several controls | `tests/corpus/195-comctl-refine.vbp` (run by WFC) | `tests/tc-0017-comctl-refine.tex` |
| `TC-MP0002-corpus-222-comctl-selection` | `TC-0018-comctl-selection` | Tab selection, panel limit and bound list | `tests/corpus/222-comctl-selection.vbp` (run by WFC) | `tests/tc-0018-comctl-selection.tex` |
| `TC-MP0002-corpus-223-comctl-toolbar-rules` | `TC-0019-comctl-toolbar-rules` | Toolbar rules | `tests/corpus/223-comctl-toolbar-rules.vbp` (run by WFC) | `tests/tc-0019-comctl-toolbar-rules.tex` |
| `TC-MP0002-corpus-224-comctl-tree-rules` | `TC-0020-comctl-tree-rules` | TreeView rules (corpus) | `tests/corpus/224-comctl-tree-rules.vbp` (run by WFC) | `tests/tc-0020-comctl-tree-rules.tex` |
| `TC-MP0002-corpus-225-comctl-list-rules` | `TC-0021-comctl-list-rules` | ListView rules (corpus) | `tests/corpus/225-comctl-list-rules.vbp` (run by WFC) | `tests/tc-0021-comctl-list-rules.tex` |
| `TC-MP0002-corpus-226-comctl-image-rules` | `TC-0022-comctl-image-rules` | ImageList and ImageCombo rules (corpus) | `tests/corpus/226-comctl-image-rules.vbp` (run by WFC) | `tests/tc-0022-comctl-image-rules.tex` |
| `TC-MP0006-gui-comctl` | `TC-0023-gui-comctl` | Common controls example in a window | `examples/common-controls/gui-comctl.vbp` | `tests/tc-0023-gui-comctl.tex` |
| `TC-MP0006-comctl-tables-current` | `TC-0024-tables-current` | Member table matches the type-library evidence | `src/mscomctl_tables.cpp` hash, checked by WFC | `tests/tc-0024-tables-current.tex` |

The corpus cases are registered by WFC under their file names
(`TC-MP0002-corpus-<file name>`); that registration is WFC's and still
uses the old names until WFC takes the library's identifiers.

## Planned test identifiers that never existed

The behavioral requirements named a planned test case with the same number
as the requirement (`TC-0010` to `TC-0036`, VB6 reference and WFC
conformance tests). None of them was ever written. Those numbers now belong
to other test cases of this library (above); the planned probes have no
identifier yet and get the next free `TC-NNNN` when they are written.

## Where the old identifiers are cited

The WFC repository documents that cite the old requirement numbers or test
names are listed in [WSP-ADOPTION.md](WSP-ADOPTION.md#identifier-migration).
