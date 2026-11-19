# wfc-mscomctl Repository Instructions

This is the Windows Common Controls library (MSComctlLib) of WFC. It follows
the WSP baseline pinned in `wsp/`; the adoption record is
`docs/WSP-ADOPTION.md`. WFC (`Thewafflication/wfc`) carries this repository as
a submodule under `lib/`, runs the corpus programs in `tests/corpus`, and holds
the reference evidence the requirements cite.

## Owner Authorization

- Ordinary file edits within this repository and the current task scope do not
  require asking the owner for approval each time.
- Running the local build and test tooling is pre-authorized: `cmake`
  (configure and build), `ctest`, and `scripts/Invoke-Lint.ps1`.
- Run only the x64 configure, build and test locally
  (`pwsh -NoProfile -File scripts/Invoke-BuildTest.ps1`). Do not run local x86
  builds or ARM64 cross-builds; GitHub Actions validates those targets after a
  push.
- Commit and push only when the owner asks. Tags and releases are the owner's
  decision; a version tag runs the shared release workflow.
- Never modify the `wsp/` submodule from here; change WSP in its own
  repository and move the pin deliberately.
- Platform-enforced approval prompts must still be honored. Destructive actions
  and actions outside the repository or current task scope require their normal
  explicit authorization.

## Commit gate

`.pre-commit-config.yaml` is the gate (`pre-commit install` once):

1. `scripts/Invoke-Lint.ps1`: WSP physical style
   (`wsp/tools/Test-SourceStyle.ps1`, 80 columns), clang-format 22.1.3,
   gersemi 0.29.2, PSScriptAnalyzer 1.25.0, YAML syntax, and pre-commit
   validation. Changes no files.
2. `scripts/Invoke-BuildTest.ps1`: x64 debug configure, build and `ctest`.

Install the pinned tools with
`python -m pip install --requirement requirements-dev.txt`. Never skip the
gate with `--no-verify`. To format, run `clang-format -i --style=file <file>`
and `python -m gersemi -i --definitions cmake wsp/tools/cmake -- <file>`.

## Style

- C and C++ follow `.clang-format` (WSP-LANG-0002): 4-space indent, 80
  columns, braces on every control body.
- CMake follows `.gersemirc` (WSP-LANG-0010); LF line endings everywhere
  (`.gitattributes`, `.editorconfig`).
- `tests/corpus` holds Visual Basic 6 programs and their expected output. They
  are fixtures, outside the source style, and must not be reformatted.
- `include/wfc/mscomctl/mscomctl.inc` and `src/mscomctl_tables.cpp` are the
  generated member tables (from WFC's `scripts/New-UiControlTables.ps1`); do
  not edit them by hand.

## Requirements and test cases

Requirements are in `requirements/` (`REQ-NNNN`, one file each, named
`req-NNNN-<slug>.md`). Test cases are named `TC-NNNN` after the requirement
they verify; the corpus programs and the CTest unit tests are their
implementations. Keep identifiers stable: never renumber or reuse one.
