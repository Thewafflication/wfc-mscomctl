# REQ-0043 — ImageCombo editing lock

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbprolockedproperty.htm`

## Scope

This requirement applies to user editing of ImageCombo text on supported Windows targets.

## Requirement

The WFC ImageCombo shall permit the user to edit its text while `Locked` is
false and shall prevent user editing while `Locked` is true.

## Rationale

The lock state changes user interaction without preventing programmatic access.

## Verification

**Method:** Test

**References:** `TC-0015`

The test cases listed check WFC's behavior against the outcomes recorded in them. The VB6 reference probe that compares the installed control with WFC is still planned; until it runs, the requirement stays proposed.

## Relationships

- **Derived from:** Installed `cmctl198.chm` documentation and `REQ-0009`
- **Depends on:** ImageCombo API contract
- **Conflicts with:** None known

## Tailoring

None.

## Implementation Record

Not implemented.
