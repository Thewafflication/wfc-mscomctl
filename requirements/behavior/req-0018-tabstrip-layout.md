# REQ-0018 — TabStrip layout behavior

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjtabcontrol.htm`; `vbpromultirow.htm`; `vbprotabwidthstyle.htm`; `vbprotabstyleproperty.htm`

## Scope

This requirement applies to TabStrip style, client-area, row, and width behavior on supported Windows targets.

## Requirement

The WFC TabStrip shall reproduce the reference layout behavior for `Style`, `MultiRow`, `TabWidthStyle`, `TabFixedWidth`, `TabFixedHeight`, placement, and client-area measurements, including wrapping tabs when `MultiRow` is true and presenting an overflow spin control when it is false.

## Rationale

Existing forms depend on the control's calculated tab rows and client rectangle to position their page containers.

## Verification

**Method:** Test

**References:** `TC-0003`, `TC-0011`

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

