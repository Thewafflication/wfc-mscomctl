<#
.SYNOPSIS
Runs test case TC-0004 (items).
.DESCRIPTION
Runs the CTest test TC-0004 in a built tree.
See tests/tc-0004-items.tex.
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
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0004' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
