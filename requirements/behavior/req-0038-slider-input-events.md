# REQ-0038 — Slider input and event timing

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjslider.htm`; `vbevtscrollslider.htm`

## Scope

This requirement applies to mouse and keyboard changes to a Slider value on supported Windows targets.

## Requirement

The WFC Slider shall allow movement by dragging, clicking beside the thumb, or keyboard input; while movement continues it shall raise `Scroll` before `Click`, and after the value change completes it shall raise `Change` once.

## Rationale

Applications distinguish continuous feedback from a completed value change using the documented event timing.

## Verification

**Method:** Test

**References:** `TC-0010`, `TC-0017`

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

