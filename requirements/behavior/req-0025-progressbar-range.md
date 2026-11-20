# REQ-0025 — ProgressBar range and rendering

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjprogressbar.htm`

## Scope

This requirement applies to ProgressBar `Min`, `Max`, and `Value` state on supported Windows targets.

## Requirement

The WFC ProgressBar shall represent progress with an inclusive range bounded by `Min` and `Max`, position it using `Value`, and render a filled extent that approximates the value using chunks determined by the control dimensions and border.

## Rationale

The reference explicitly distinguishes numerical state from the chunked visual approximation.

## Verification

**Method:** Test

**References:** `TC-0002`, `TC-0009`, `TC-0010`

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

