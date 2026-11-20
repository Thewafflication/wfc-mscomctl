<#
.SYNOPSIS
Runs test case TC-0016 (comctl-persisted).
.DESCRIPTION
Runs the corpus program 194-comctl-persisted through wfc
and compares its output. See tests/tc-0016-comctl-persisted.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '194-comctl-persisted' -Wfc $Wfc
