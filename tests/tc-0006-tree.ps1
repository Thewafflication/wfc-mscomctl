<#
.SYNOPSIS
Runs test case TC-0006 (tree).
.DESCRIPTION
Runs the CTest test TC-0006 in a built tree.
See tests/tc-0006-tree.tex.
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
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0006' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
