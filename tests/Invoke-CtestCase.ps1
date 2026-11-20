<#
.SYNOPSIS
Runs one CTest test case of the library by its TC identifier.
.DESCRIPTION
Runs the CTest test whose name starts with the identifier (for example
TC-0002-range for TC-0002; wfc-mscomctl-TC-0002-range when WFC added the
library, so the name may carry the wfc-mscomctl- prefix) in an already built
tree. Fails when no test matches, so a case that is not built (the DLL test
needs a shared build) is not reported as passed.
.PARAMETER Id
The test case identifier, TC-NNNN.
.PARAMETER BuildDirectory
The CMake build tree, relative to the repository root.
.PARAMETER Configuration
The configuration to run.
.EXAMPLE
pwsh -NoProfile -File tests/tc-0002-range.ps1
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidatePattern('^TC-\d{4}$')][string]$Id,
    [string]$BuildDirectory = 'out/build',
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot '../scripts/LibChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    Invoke-CheckedCommand ctest @(
        '--test-dir', $BuildDirectory, '-C', $Configuration,
        '-R', "^(wfc-mscomctl-)?$Id-", '--no-tests=error',
        '--output-on-failure')
}
finally {
    Pop-Location
}
