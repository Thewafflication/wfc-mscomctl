<#
.SYNOPSIS
Runs test case TC-0012 (comctl-toolbar).
.DESCRIPTION
Runs the corpus program 190-comctl-toolbar through wfc
and compares its output. See tests/tc-0012-comctl-toolbar.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '190-comctl-toolbar' -Wfc $Wfc
