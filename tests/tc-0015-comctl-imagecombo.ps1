<#
.SYNOPSIS
Runs test case TC-0015 (comctl-imagecombo).
.DESCRIPTION
Runs the corpus program 193-comctl-imagecombo through wfc
and compares its output. See tests/tc-0015-comctl-imagecombo.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '193-comctl-imagecombo' -Wfc $Wfc
