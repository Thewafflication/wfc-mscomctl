# REQ-0036 — ImageList rendering and composition

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbmthdraw.htm`; `vbmthextracticon.htm`; `vbmthoverlay.htm`

## Scope

This requirement applies to drawing, extracting, and overlaying ImageList images on supported Windows targets.

## Requirement

The WFC ImageList shall draw images in normal, mask-transparent, selected, and focus styles; extract a usable icon from an image; and overlay two index- or key-selected images using `MaskColor` for transparency and return the composite picture.

## Rationale

These operations are the observable output behaviors beyond using the control as an image repository.

## Verification

**Method:** Test

**References:** `TC-0008`, `TC-0017`, `TC-0022`

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

