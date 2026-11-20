<#
.SYNOPSIS
Runs test case TC-0023 (common controls example in a window).
.DESCRIPTION
Runs examples/common-controls/gui-comctl.vbp through wfc in its Win32 host
and takes the screenshots; see tc-0023-gui-comctl.tex. Needs a desktop
session that no other test is using.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
.PARAMETER ScreenshotDirectory
Where wfc writes the screenshots, relative to the repository root.
#>
[CmdletBinding()]
param(
    [string]$Wfc,
    [string]$ScreenshotDirectory = 'out/screenshots'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = Split-Path -Parent $PSScriptRoot
$wfcPath = & (Join-Path $PSScriptRoot 'Resolve-Wfc.ps1') -Wfc $Wfc
$project = Join-Path $root 'examples/common-controls/gui-comctl.vbp'
$directory = Join-Path $root $ScreenshotDirectory
New-Item -ItemType Directory -Force -Path $directory | Out-Null
$output = (& $wfcPath --interpret $project --gui --screenshots $directory |
        Out-String)
if ($LASTEXITCODE -ne 0) {
    throw "wfc exited with $LASTEXITCODE."
}
if ($output -notmatch 'closing') {
    throw "The example did not report 'closing':`n$output"
}
Write-Output "[PASS] gui-comctl; screenshots in $directory"
