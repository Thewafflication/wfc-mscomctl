<#
.SYNOPSIS
Runs test case TC-0018 (comctl-selection).
.DESCRIPTION
Runs the corpus program 222-comctl-selection through wfc
and compares its output. See tests/tc-0018-comctl-selection.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '222-comctl-selection' -Wfc $Wfc
