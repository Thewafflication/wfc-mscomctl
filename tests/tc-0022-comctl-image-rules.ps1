<#
.SYNOPSIS
Runs test case TC-0022 (comctl-image-rules).
.DESCRIPTION
Runs the corpus program 226-comctl-image-rules through wfc
and compares its output. See tests/tc-0022-comctl-image-rules.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '226-comctl-image-rules' -Wfc $Wfc
