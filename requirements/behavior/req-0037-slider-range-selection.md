# REQ-0037 — Slider value and selection range

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjslider.htm`; `vbproselectrange.htm`; `vbprosellengthslider.htm`

## Scope

This requirement applies to Slider range, selection, and tick state on supported Windows targets.

## Requirement

The WFC Slider shall constrain `Value`, `SelStart`, and `SelLength` to its `Min`/`Max` range, require nonnegative `SelLength` with `SelStart + SelLength` not exceeding `Max`, couple `SelStart` and `Value` while `SelectRange` is false, and make `SelLength` ineffective in that mode.

## Rationale

These invariants define the numerical state represented by the slider.

## Verification

**Method:** Test

**References:** `TC-0002`, `TC-0010`, `TC-0017`

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

