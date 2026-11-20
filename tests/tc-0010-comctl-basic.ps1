<#
.SYNOPSIS
Runs test case TC-0010 (comctl-basic).
.DESCRIPTION
Runs the corpus program 188-comctl-basic through wfc
and compares its output. See tests/tc-0010-comctl-basic.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '188-comctl-basic' -Wfc $Wfc
