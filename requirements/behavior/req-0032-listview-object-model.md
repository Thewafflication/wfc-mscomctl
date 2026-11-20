# REQ-0032 — ListView item and column model

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjlistview.htm`; `vbmthaddlistitems.htm`; `vbmthaddmethod(listsubitemscollection).htm`; `vbmthaddcolumnheaders.htm`

## Scope

This requirement applies to the ListView `ListItems`, `ListSubItems`, and `ColumnHeaders` collections on supported Windows targets.

## Requirement

The WFC ListView shall represent each entry as a `ListItem`, its additional report columns as ordered subitems, and its report headings as `ColumnHeader` objects that may be added at run time and resized by the user.

## Rationale

VB6 applications construct and update ListView contents through these object collections.

## Verification

**Method:** Test

**References:** `TC-0014`, `TC-0021`

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

