# WSP Adoption Record

**Content type:** Project adoption record

**Project:** wfc-mscomctl (Windows Common Controls)

**WSP release:** 1.4.0

**WSP baseline:** Immutable commit `f009399dd1406219571ac978bfee85e85bbdfeac`

**Submodule path:** `wsp/`

**Pinned commit:** `f009399dd1406219571ac978bfee85e85bbdfeac`

**Status:** Adopted; the common practices are in place and the open items are
listed below

The library is one of the WFC libraries and follows the WFC adoption record
(`planning/wsp-adoption-record.md` in
[WFC](https://github.com/Thewafflication/wfc)); this record covers what the
library repository itself carries.

## Selected profiles

| Profile | Selected | Scope or rationale |
| --- | --- | --- |
| Security/DFS | Yes | Hardening flags (WSP-SEC-0015) from `wsp/tools/cmake/WspBuild.cmake` in the standalone build |
| C source style | No | The sources are C++; the C test programs follow the same layout |
| CMake style | Yes | `CMakeLists.txt` |
| Windows version resources | Yes | `version.rc.in`, one version resource per DLL |
| Windows code signing and Defender | Deferred | Until the first release |
| Documentation | Yes | `README.md`, `ROADMAP.md`, `COMPATIBILITY.md` |

## Gap analysis (2026-11-19)

WSP 1.4.0 is the version the WFC repository pins, and this library pins the
same commit. Status: Done, Partial, Open or Decision (the owner decides).

| Item | WSP source | Status | Action |
| --- | --- | --- | --- |
| WSP submodule at the WFC version | `wsp/README.md` | Done | `wsp/` at 1.4.0 (`f009399`), same as WFC |
| Adoption record | `wsp/requirements/adoption-record-template.md` | Partial | This file; the per-requirement-set dispositions of the WFC record are not repeated (see Decision 3) |
| C and C++ formatter | WSP-LANG-0002, `wsp/style/language-style.md` | Done | `.clang-format` from WFC; clang-format 22.1.3 passes on every owned file |
| CMake formatter | WSP-LANG-0010, `wsp/style/cmake-style.md` | Done | `.gersemirc` from WFC; gersemi 0.29.2 passes (project definitions passed with `--definitions`) |
| Editor and line-ending defaults | WSP-STYLE-0002 to 0004 | Done | `.editorconfig` added; `.gitattributes` already fixes LF |
| 80-column physical style | WSP-STYLE-0001, `wsp/tools/Test-SourceStyle.ps1` | Done | 23 findings in 6 files fixed by hand or clang-format; 46 owned files pass |
| Lint stage | WSP-CHECK-0001, `wsp/processes/commit-checks.md` | Done | `scripts/Invoke-Lint.ps1` (style, clang-format, gersemi, PSScriptAnalyzer, YAML, pre-commit) |
| Commit gate | WSP-CHECK-0001 to 0008 | Partial | `.pre-commit-config.yaml` runs lint, then `scripts/Invoke-BuildTest.ps1` (x64 debug); run `pre-commit install` once per clone. No progress window or single-instance lock (WFC-specific) |
| Pinned tools | WSP-CHECK-0007 | Done | `requirements-dev.txt` (pre-commit, gersemi, clang-format); PSScriptAnalyzer 1.25.0 is checked by the lint script |
| CI runs the lint | WSP-CHECK-0008 | Open | Add a lint job to `.github/workflows/ci.yml` (it builds and tests on x86, x64 and ARM64 only) |
| Requirements with stable IDs | WSP-REQM-0001 to 0010, `wsp/requirements` | Done | `requirements/` has `REQ-0001` to `REQ-0043` in the library's own namespace (decision 1); the old numbers are in `docs/id-map.md` |
| Test cases with stable IDs | WSP-TEST-0001 to 0005 | Done | `TC-0001` to `TC-0024`, each with a `tests/tc-NNNN-*.tex` specification and a `tests/tc-NNNN-*.ps1` runner; CTest names are `TC-NNNN-<name>` (decision 2) |
| Traceability check | WSP-REQM, WSP-TEST-0011, `wsp/tools/Test-Traceability.ps1` | Partial | `scripts/Invoke-Traceability.ps1` passes for 42 of 43 requirements; `REQ-0026` (ProgressBar orientation and scrolling) has no test case yet, so the check fails and is not in the gate or CI until it does |
| Test evidence and reports | WSP-TEST-0006 to 0018 | Open | Deferred, as in WFC |
| Version resource | WSP-WINRES-0001 to 0012 | Partial | `version.rc.in` exists; no `Test-PeHardening` or resource verification in the gate yet |
| Hardening flags | WSP-SEC-0015 | Done | `wsp_enable_hardening` in the standalone build |
| Static analysis | WSP-SAST-0001 to 0006 | Open | `wsp/tools/static-analysis/wsp-clang-tidy.yml` not used yet |
| Signing and Defender | WSP-SIGN-0001 to 0018 | Open | Deferred to the first release (shared WFC release workflow) |
| Documentation structure | WSP-DOC, `wsp/style/documentation-style.md` | Partial | `README.md`, `ROADMAP.md`, `COMPATIBILITY.md`; no release document pipeline |
| Release and versioning | `wsp/processes/software-lifecycle.md` | Partial | `VERSION` plus the shared tag-driven release workflow; no `CHANGELOG`, release-readiness record or milestone plans |
| AGENTS.md | WFC model | Done | Added |

## Decisions for the owner

1. **Requirement numbering (decided).** The library mixes ranges: `REQ-0001` to `0036`
   restart at 1 (collisions are possible with WFC's own `REQ-` numbers, for
   example WFC has `req-0140` and the library has `req-0296`), while
   `REQ-0296` to `0302` were taken from WFC's sequence. Choose between one
   global namespace (WFC allocates the numbers; the library keeps what it
   has) and a per-library prefix such as `REQ-COMCTL-0001` (WSP-REQM
   requires stable, unique identifiers).
2. **Test-case layout (decided).** Either adopt the `Test-Traceability.ps1` layout
   (`tests/tc-NNNN-*.tex` plus `tc-NNNN.ps1` runners, which suits a library
   with its own test suite) or keep the test cases in WFC and run the
   traceability check from WFC across the libraries.
3. **Adoption record depth.** The WFC record lists a disposition per WSP
   requirement set; this one lists only profiles. Say whether the library
   should carry its own full record.
4. **CI.** Add the lint job (and later the traceability check) to
   `ci.yml`, and enable the pre-commit hook in the owner's clone.

## Identifier migration

Decisions 1 and 2 were taken as: `REQ-NNNN` and `TC-NNNN` from 0001 in the
library's own namespace, with the `tc-NNNN` layout of the traceability tool.
The requirements are `REQ-0001` to `REQ-0009` (API contracts, unchanged),
`REQ-0010` to `REQ-0016` (the former `REQ-0296` to `REQ-0302`) and
`REQ-0017` to `REQ-0043` (the former behavioral `REQ-0010` to `REQ-0036`):
43 in all. There are 24 test cases: `TC-0001` to `TC-0009` (CTest, from the
former `TC-MP0010-mscomctl-*`), `TC-0010` to `TC-0022` (the 13 corpus
programs, run by WFC), `TC-0023` (the example in a window) and `TC-0024`
(the table check against the evidence, run by WFC). All 24 have a LaTeX
specification (24 created) and a PowerShell runner; the corpus runners
(`tests/Invoke-CorpusCase.ps1`) need a `wfc.exe` and were run against WFC's
installed debug build, all passing. `docs/id-map.md` maps every old
identifier and title to the new one and notes that the planned `TC-0010` to
`TC-0036` of the old behavioral requirements never existed.

WFC documents that cite the old identifiers (not edited from here):
`planning/MP-0001-foundation-and-feasibility.md`,
`planning/MP-0006-closeout.md`, `planning/MP-0006-coverage.md`,
`planning/MP-0006-mscomctllib.md`, `planning/work-log-mp-0006.md`,
`planning/MP-0010-distribution-and-libraries.md`, `planning/roadmap-1.0.md`,
`requirements/README.md`, `requirements/language/diagnostics.md` (`REQ-0302`
for `WFC0293`), `architecture/adr-0010-comctl-item-model.md` and the root
`CMakeLists.txt` (the `TC-MP0006-gui-comctl` and
`TC-MP0006-comctl-tables-current` tests, and the corpus registration
`TC-MP0002-corpus-<file>`, which keeps the old names until WFC adopts the
library identifiers). WFC's own requirements with the same numbers (for
example its `REQ-0037`, the PictureBox contract) are unrelated.

## Open items

| Item | Disposition |
| --- | --- |
| 80-column style (WSP-STYLE-0001) | Met: `scripts/Invoke-Lint.ps1` passes; CI does not run it yet |
| Requirements | Adopted: `requirements/` (moved from WFC) |
| Traceability check | `scripts/Invoke-Traceability.ps1`; fails only for `REQ-0026`, which needs a ProgressBar rendering test; then add to the gate and CI |
| Signing | Planned for the first release |
