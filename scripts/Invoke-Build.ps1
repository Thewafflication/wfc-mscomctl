<#
.SYNOPSIS
Configures and builds the library standalone (x64 debug).
.DESCRIPTION
Runs cmake configure and build in out/build. Local runs cover x64 only; CI
validates x86 and ARM64 after a push. Stops at the first failing step.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Build.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'LibChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    Write-Stage '[build] configure x64 Debug'
    Invoke-CheckedCommand cmake @('-S', '.', '-B', 'out/build', '-A', 'x64')
    Write-Stage '[build] compile x64 Debug'
    Invoke-CheckedCommand cmake @('--build', 'out/build', '--config', 'Debug')
    Write-Output '[PASS] build x64 Debug'
}
finally {
    Pop-Location
}
