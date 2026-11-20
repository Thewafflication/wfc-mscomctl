<#
.SYNOPSIS
Runs test case TC-0001 (tables).
.DESCRIPTION
Runs the CTest test TC-0001 in a built tree.
See tests/tc-0001-tables.tex.
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
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0001' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
