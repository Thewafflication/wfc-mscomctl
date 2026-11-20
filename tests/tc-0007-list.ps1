<#
.SYNOPSIS
Runs test case TC-0007 (list).
.DESCRIPTION
Runs the CTest test TC-0007 in a built tree.
See tests/tc-0007-list.tex.
.PARAMETER BuildDirectory
The CMake build tree, relative to the repository root.
.PARAMETER Configuration
The configuration to run.
#>
[CmdletBinding()]
param(
    [string]$BuildDirectory = 'out/build',
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0007' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
