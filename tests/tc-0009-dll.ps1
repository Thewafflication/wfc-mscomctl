<#
.SYNOPSIS
Runs test case TC-0009 (dll).
.DESCRIPTION
Runs the CTest test TC-0009 in a built tree.
See tests/tc-0009-dll.tex.
This case runs only in a shared build on Windows: configure with
-DWFC_MSCOMCTL_SHARED=ON and pass -BuildDirectory.
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
& (Join-Path $PSScriptRoot 'Invoke-CtestCase.ps1') -Id 'TC-0009' `
    -BuildDirectory $BuildDirectory -Configuration $Configuration
