# REQ-0028 — TreeView keyboard navigation

**Content type:** Project requirement  
**Status:** Proposed  
**Source:** Microsoft Common Controls 6.0 installed help, `vbobjtreeview.htm`

## Scope

This requirement applies to keyboard input while a TreeView has focus on supported Windows targets.

## Requirement

The WFC TreeView shall navigate expanded visible nodes with Up and Down, expand or advance with Right, collapse with Left, wrap from the bottom to the top, and cycle among visible nodes beginning with a typed ANSI character.

## Rationale

Keyboard behavior is externally observable and required for compatible interaction and accessibility.

## Verification

**Method:** Test

**References:** `TC-0006`, `TC-0013`, `TC-0020`

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

