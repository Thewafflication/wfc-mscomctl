# REQ-0019 — TabStrip pre-selection cancellation

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbevtbeforeclick.htm`

## Scope

This requirement applies to a user or programmatic change to the selected TabStrip tab on supported Windows targets.

## Requirement

Before changing the selected tab, the WFC TabStrip shall raise `BeforeClick` with `Cancel` initially zero; a nonzero `Cancel` shall prevent focus from changing to the new tab but shall not suppress the subsequent `Click` event.

## Rationale

VB6 applications use this event to validate the old page before permitting a selection change.

## Verification

**Method:** Test

**References:** `TC-0011`, `TC-0017`

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

