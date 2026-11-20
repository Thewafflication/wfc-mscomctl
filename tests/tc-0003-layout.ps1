<#
.SYNOPSIS
Runs test case TC-0003 (layout).
.DESCRIPTION
Runs the CTest test TC-0003 in a built tree.
See tests/tc-0003-layout.tex.
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
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0003' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
