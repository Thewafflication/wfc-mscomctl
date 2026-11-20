<#
.SYNOPSIS
Runs test case TC-0014 (comctl-listview).
.DESCRIPTION
Runs the corpus program 192-comctl-listview through wfc
and compares its output. See tests/tc-0014-comctl-listview.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '192-comctl-listview' -Wfc $Wfc
