# REQ-0031 — ListView view modes

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjlistview.htm`; `vbproview.htm`; `vbprolabelwrap.htm`

## Scope

This requirement applies to the ListView `View` property and associated presentation on supported Windows targets.

## Requirement

The WFC ListView shall implement Icon, SmallIcon, List, and Report views with the documented icon and label arrangement; Report view shall place item text and subitems into columns, and Icon view shall honor `LabelWrap`.

## Rationale

The four presentation modes are the defining behavior of the ListView control.

## Verification

**Method:** Test

**References:** `TC-0007`, `TC-0021`

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

