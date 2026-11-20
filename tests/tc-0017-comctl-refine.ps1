<#
.SYNOPSIS
Runs test case TC-0017 (comctl-refine).
.DESCRIPTION
Runs the corpus program 195-comctl-refine through wfc
and compares its output. See tests/tc-0017-comctl-refine.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '195-comctl-refine' -Wfc $Wfc
