# Corpus of wfc-mscomctl

The corpus programs here are run by [WFC](https://github.com/Thewafflication/wfc)
(`wfc` is the interpreter and compiler these controls belong to): each
`NN-name.vbp` must print exactly the matching `.expected` text (nothing, for
these form programs) and the Immediate-window text of the `.debug` file; a
`.ui` file scripts the user's input. They are the implementations of test
cases `TC-0010` to `TC-0022` (the case of each file is in
[../README.md](../README.md)), specified in `tests/tc-NNNN-*.tex` and run by
`tests/tc-NNNN-*.ps1` with `-Wfc <path to wfc.exe>`, the `WFC_EXE` variable
or `wfc` on `PATH`. WFC itself registers each program as the CTest case
`TC-MP0002-corpus-NN-name`, a name from before the library had its own
identifiers (`docs/id-map.md`).

The C and C++ tests of the library itself are in `tests/` and run in this
repository's CI.
