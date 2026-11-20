<#
.SYNOPSIS
Runs the WSP traceability check on the requirements and test cases.
.DESCRIPTION
Calls wsp/tools/Test-Traceability.ps1 with this library's layout:
requirements/**/req-NNNN-*.md, test specifications tests/tc-NNNN-*.tex.
The test cases are CTest programs, WFC corpus programs and an example rather
than tc-NNNN PowerShell runners, so the check runs with -AllowManualTests:
each requirement must name existing TC-NNNN cases and each specification must
name the requirements it verifies, but a PowerShell runner per case is not
required.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Traceability.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'LibChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    Write-Stage '[trace] WSP traceability'
    Invoke-CheckedCommand pwsh @(
        '-NoProfile', '-File', 'wsp/tools/Test-Traceability.ps1',
        '-RepositoryRoot', '.',
        '-RequirementsPath', 'requirements',
        '-TestSpecificationsPath', 'tests',
        '-TestImplementationsPath', 'tests',
        '-AllowManualTests')
}
finally {
    Pop-Location
}
