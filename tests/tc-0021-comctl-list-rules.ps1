<#
.SYNOPSIS
Runs test case TC-0021 (comctl-list-rules).
.DESCRIPTION
Runs the corpus program 225-comctl-list-rules through wfc
and compares its output. See tests/tc-0021-comctl-list-rules.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '225-comctl-list-rules' -Wfc $Wfc
