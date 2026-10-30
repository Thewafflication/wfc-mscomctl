# wfc-mscomctl: compatibility with mscomctl.ocx

Target: **MSCOMCTL.OCX 6.1.98 (Visual Basic 6.0 SP6)**. Library version: **0.6.1**.

Generated from the type library of MSCOMCTL.OCX 2.0 retained in the WFC evidence.

## Surface

| Class | Members | Events | Table | Behaviour |
| --- | ---: | ---: | --- | --- |
| TabStrip | 57 | 14 | listed | in the WFC evaluator |
| Toolbar | 51 | 15 | listed | in the WFC evaluator |
| StatusBar | 26 | 13 | listed | in the WFC evaluator |
| ProgressBar | 29 | 10 | listed | in the WFC evaluator |
| TreeView | 61 | 20 | listed | in the WFC evaluator |
| ListView | 90 | 19 | listed | in the WFC evaluator |
| ImageList | 16 | 0 | listed | in the WFC evaluator |
| Slider | 47 | 15 | listed | in the WFC evaluator |
| ImageCombo | 44 | 12 | listed | in the WFC evaluator |

9 class(es), 421 members and 118 events are in
the table. *Table* means the member exists with the Microsoft name, kind, type
and dispatch id. *Behaviour* is implemented by the WFC evaluator and covered by
its corpus and GUI tests (`TC-MP0006-*`, `TC-MP0010-*`); per-member behaviour
status is tracked in WFC until the behaviour moves into this library (see the
[roadmap](ROADMAP.md), 0.7).

## Registration

| Item | WFC | Microsoft |
| --- | --- | --- |
| ProgID and class id | `WFC.MSComctlLib.<Class>`, a fixed UUID per class | `MSComctlLib.<Class>`, the Microsoft class id |
| Selected by | the default | `WFC_REGISTER_MICROSOFT_IDS=1` (the previous server is saved and restored on unregister) |
| COM creation | `DllGetClassObject` reports the class as not available | n/a |

## Known gaps

- Per-member behaviour is not measured yet; the table lists every member of the type library, and the behaviour is that of the WFC evaluator.
- `DllGetClassObject` does not create controls yet; a program that creates the
  control through COM does not get one from this library (roadmap 0.7).

## How it is checked

- A C program (`tests/mscomctl_dll_test.c`) loads the DLL, checks its COM entry
  points and version resource, and registers and unregisters it.
- A table test (`tests/mscomctl_tables_test.cpp`) checks the classes and the
  member kinds.
- WFC compares the table with the type-library evidence
  (`TC-MP0006-comctl-tables-current` for the generated tables) and runs its
  corpus programs against the controls.
