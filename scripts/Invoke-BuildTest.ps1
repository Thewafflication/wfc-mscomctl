<#
.SYNOPSIS
Configures, builds and tests the library standalone (x64 debug).
.DESCRIPTION
Runs cmake configure and build, then ctest, in out/build. Local runs cover x64
only; CI validates x86 and ARM64 after a push. Stops at the first failing step.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-BuildTest.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'LibChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    Write-Stage '[build] cmake configure'
    Invoke-CheckedCommand cmake @('-S', '.', '-B', 'out/build', '-A', 'x64')
    Write-Stage '[build] cmake build'
    Invoke-CheckedCommand cmake @('--build', 'out/build', '--config', 'Debug')
    Write-Stage '[test] ctest'
    Invoke-CheckedCommand ctest @(
        '--test-dir', 'out/build', '-C', 'Debug', '--output-on-failure')
    Write-Output '[PASS] build and tests'
}
finally {
    Pop-Location
}
