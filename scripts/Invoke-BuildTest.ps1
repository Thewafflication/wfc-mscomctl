<#
.SYNOPSIS
Configures, builds and tests the library standalone (x64 debug).
.DESCRIPTION
Runs scripts/Invoke-Build.ps1, then scripts/Invoke-Tests.ps1 (which logs for
the progress window). Local runs cover x64 only; CI validates x86 and ARM64
after a push. Stops at the first failing step.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-BuildTest.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

& (Join-Path $PSScriptRoot 'Invoke-Build.ps1')
& (Join-Path $PSScriptRoot 'Invoke-Tests.ps1')
Write-Output '[PASS] build and tests'
