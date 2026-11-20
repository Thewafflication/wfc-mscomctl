<#
.SYNOPSIS
Runs test case TC-0011 (comctl-tabstatus).
.DESCRIPTION
Runs the corpus program 189-comctl-tabstatus through wfc
and compares its output. See tests/tc-0011-comctl-tabstatus.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '189-comctl-tabstatus' -Wfc $Wfc
