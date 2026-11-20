# Test cases of wfc-mscomctl

**Content type:** Project test index

Each test case `TC-NNNN` has a specification (`tests/tc-NNNN-<name>.tex`,
built from `wsp/testing/test-case-template.tex`) that states its purpose, the
requirements it verifies, its procedure and expected result. The numbers are
the library's own; `docs/id-map.md` lists the identifiers they replace.

| Test case | Kind | Implementation | Requirements |
| --- | --- | --- | --- |
| [`TC-0001`](tc-0001-tables.tex) Member table of the common controls | CTest unit | `tests/mscomctl_tables_test.cpp` | `REQ-0001`, `REQ-0002`, `REQ-0003`, `REQ-0004`, `REQ-0005`, `REQ-0006`, `REQ-0007`, `REQ-0008`, `REQ-0009` |
| [`TC-0002`](tc-0002-range.tex) Range rules of ProgressBar and Slider | CTest unit | `tests/range_test.cpp` | `REQ-0010`, `REQ-0025`, `REQ-0037` |
| [`TC-0003`](tc-0003-layout.tex) Layout of tabs and toolbar buttons | CTest unit | `tests/layout_test.cpp` | `REQ-0011`, `REQ-0018`, `REQ-0020` |
| [`TC-0004`](tc-0004-items.tex) Keys, indexes and collection errors | CTest unit | `tests/items_test.cpp` | `REQ-0010`, `REQ-0011`, `REQ-0017` |
| [`TC-0005`](tc-0005-toolbar.tex) Toolbar button rules | CTest unit | `tests/toolbar_test.cpp` | `REQ-0012`, `REQ-0020`, `REQ-0022` |
| [`TC-0006`](tc-0006-tree.tex) TreeView rules | CTest unit | `tests/tree_test.cpp` | `REQ-0013`, `REQ-0027`, `REQ-0028`, `REQ-0029`, `REQ-0030` |
| [`TC-0007`](tc-0007-list.tex) ListView rules | CTest unit | `tests/list_test.cpp` | `REQ-0014`, `REQ-0031`, `REQ-0033`, `REQ-0034` |
| [`TC-0008`](tc-0008-images.tex) ImageList and ImageCombo rules | CTest unit | `tests/images_test.cpp` | `REQ-0010`, `REQ-0015`, `REQ-0035`, `REQ-0036`, `REQ-0040` |
| [`TC-0009`](tc-0009-dll.tex) Library DLL: exports, version and registration | CTest integration (shared DLL, Windows) | `tests/mscomctl_dll_test.c` | `REQ-0025` |
| [`TC-0010`](tc-0010-comctl-basic.tex) Basic controls: ProgressBar, Slider, ImageList | corpus (run by WFC) | `tests/corpus/188-comctl-basic.*` | `REQ-0010`, `REQ-0025`, `REQ-0035`, `REQ-0037`, `REQ-0038`, `REQ-0039` |
| [`TC-0011`](tc-0011-comctl-tabstatus.tex) TabStrip and StatusBar | corpus (run by WFC) | `tests/corpus/189-comctl-tabstatus.*` | `REQ-0011`, `REQ-0017`, `REQ-0018`, `REQ-0019`, `REQ-0023`, `REQ-0024` |
| [`TC-0012`](tc-0012-comctl-toolbar.tex) Toolbar buttons | corpus (run by WFC) | `tests/corpus/190-comctl-toolbar.*` | `REQ-0012`, `REQ-0020`, `REQ-0022` |
| [`TC-0013`](tc-0013-comctl-treeview.tex) TreeView nodes | corpus (run by WFC) | `tests/corpus/191-comctl-treeview.*` | `REQ-0013`, `REQ-0027`, `REQ-0028`, `REQ-0029`, `REQ-0030` |
| [`TC-0014`](tc-0014-comctl-listview.tex) ListView items and columns | corpus (run by WFC) | `tests/corpus/192-comctl-listview.*` | `REQ-0014`, `REQ-0032`, `REQ-0033`, `REQ-0034`, `REQ-0041` |
| [`TC-0015`](tc-0015-comctl-imagecombo.tex) ImageCombo items and text | corpus (run by WFC) | `tests/corpus/193-comctl-imagecombo.*` | `REQ-0015`, `REQ-0040`, `REQ-0042`, `REQ-0043` |
| [`TC-0016`](tc-0016-comctl-persisted.tex) Persisted state in a form file | corpus (run by WFC) | `tests/corpus/194-comctl-persisted.*` | `REQ-0016`, `REQ-0023` |
| [`TC-0017`](tc-0017-comctl-refine.tex) Refinements of several controls | corpus (run by WFC) | `tests/corpus/195-comctl-refine.*` | `REQ-0019`, `REQ-0021`, `REQ-0022`, `REQ-0023`, `REQ-0029`, `REQ-0034`, `REQ-0035`, `REQ-0036`, `REQ-0037`, `REQ-0038`, `REQ-0039`, `REQ-0041` |
| [`TC-0018`](tc-0018-comctl-selection.tex) Tab selection, panel limit and bound list | corpus (run by WFC) | `tests/corpus/222-comctl-selection.*` | `REQ-0011`, `REQ-0017`, `REQ-0023`, `REQ-0035` |
| [`TC-0019`](tc-0019-comctl-toolbar-rules.tex) Toolbar rules | corpus (run by WFC) | `tests/corpus/223-comctl-toolbar-rules.*` | `REQ-0012`, `REQ-0020`, `REQ-0022` |
| [`TC-0020`](tc-0020-comctl-tree-rules.tex) TreeView rules (corpus) | corpus (run by WFC) | `tests/corpus/224-comctl-tree-rules.*` | `REQ-0013`, `REQ-0027`, `REQ-0028`, `REQ-0029`, `REQ-0030` |
| [`TC-0021`](tc-0021-comctl-list-rules.tex) ListView rules (corpus) | corpus (run by WFC) | `tests/corpus/225-comctl-list-rules.*` | `REQ-0014`, `REQ-0031`, `REQ-0032`, `REQ-0033`, `REQ-0034`, `REQ-0041` |
| [`TC-0022`](tc-0022-comctl-image-rules.tex) ImageList and ImageCombo rules (corpus) | corpus (run by WFC) | `tests/corpus/226-comctl-image-rules.*` | `REQ-0015`, `REQ-0035`, `REQ-0036`, `REQ-0040`, `REQ-0042` |
| [`TC-0023`](tc-0023-gui-comctl.tex) Common controls example in a window | example (run by WFC) | `examples/common-controls/gui-comctl.vbp` | `REQ-0010` |
| [`TC-0024`](tc-0024-tables-current.tex) Member table matches the type-library evidence | table check (run by WFC) | `src/mscomctl_tables.cpp` hash, checked by WFC | `REQ-0001`, `REQ-0002`, `REQ-0003`, `REQ-0004`, `REQ-0005`, `REQ-0006`, `REQ-0007`, `REQ-0008`, `REQ-0009` |

CTest runs `TC-0001` to `TC-0009` from this repository
(`ctest --test-dir out/build -C Debug`; `TC-0009` only in the shared build on
Windows). When WFC adds the library (the `lib/wfc-mscomctl` submodule) the
CTest names carry the prefix `wfc-mscomctl-` (`WFC_MSCOMCTL_TEST_PREFIX`),
for example `wfc-mscomctl-TC-0007-list`; the identifiers do not change.
[WFC](https://github.com/Thewafflication/wfc) runs `TC-0010` to
`TC-0024`: it needs its interpreter and compiler for the corpus programs and
the example, and its evidence folder for the table check.

The traceability check is `scripts/Invoke-Traceability.ps1`.
