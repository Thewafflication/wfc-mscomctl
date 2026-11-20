# REQ-0033 — ListView searching and sorting

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbmthfinditem.htm`; `vbprosortedlistview.htm`; `vbprosortkey.htm`; `vbprosortorder.htm`

## Scope

This requirement applies to the `FindItem`, `Sorted`, `SortOrder`, and `SortKey` behaviors on supported Windows targets.

## Requirement

`FindItem` shall search text, subitems, or tags from the requested one-based index using whole-word-prefix or partial-prefix matching, and the ListView shall apply `SortOrder` and `SortKey` only while `Sorted` is true.

## Rationale

Search and sort semantics affect which object is returned and the observable ordering of every item.

## Verification

**Method:** Test

**References:** `TC-0007`, `TC-0014`, `TC-0021`

The test cases listed check WFC's behavior against the outcomes recorded in them. The VB6 reference probe that compares the installed control with WFC is still planned; until it runs, the requirement stays proposed.

## Relationships

- **Derived from:** The installed `cmctl198.chm` documentation and the relevant
  Automation contract requirement in `REQ-0001` through `REQ-0009`
- **Depends on:** Reference environment record and the applicable control API contract
- **Conflicts with:** None known

## Tailoring

A documented platform difference may be accepted only through an approved
compatibility-scope or architecture decision that identifies its user impact.

## Implementation Record

Not implemented.

