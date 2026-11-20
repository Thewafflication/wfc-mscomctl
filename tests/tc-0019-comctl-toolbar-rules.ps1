<#
.SYNOPSIS
Runs test case TC-0019 (comctl-toolbar-rules).
.DESCRIPTION
Runs the corpus program 223-comctl-toolbar-rules through wfc
and compares its output. See tests/tc-0019-comctl-toolbar-rules.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '223-comctl-toolbar-rules' -Wfc $Wfc
