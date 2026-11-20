# REQ-0026 — ProgressBar orientation and fill style

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbproorientationproperty(progressbarcontrol).htm`; `vbproscrollingproperty.htm`

## Scope

This requirement applies to ProgressBar `Orientation` and `Scrolling` settings on supported Windows targets.

## Requirement

The WFC ProgressBar shall render horizontally when `Orientation` is 0 and vertically when it is 1, and shall render a segmented fill when `Scrolling` is 0 and a smooth fill when it is 1.

## Rationale

These enumerated settings define the control's two axes and two documented fill presentations.

## Verification

**Method:** Test

**References:** None yet. No test case renders a ProgressBar in each `Orientation` and `Scrolling` setting; the test case that does has to be written and specified before this requirement can be accepted.

The test shall render the control in each combination against the installed reference baseline and WFC and compare the result.

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

