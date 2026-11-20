# REQ-0022 — Toolbar state persistence

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbmthsavetoolbar.htm`; `vbmthrestoretoolbar.htm`

## Scope

This requirement applies to the `SaveToolbar` and `RestoreToolbar` methods on supported Windows targets.

## Requirement

`SaveToolbar(key, subkey, value)` shall persist the current Toolbar state under the specified Windows Registry location, creating missing locations, and `RestoreToolbar` with the same identifiers shall restore that state.

## Rationale

Persisted customization must survive process restarts and remain separated by caller-selected registry identifiers.

## Verification

**Method:** Test

**References:** `TC-0005`, `TC-0012`, `TC-0017`, `TC-0019`

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

