<#
.SYNOPSIS
Runs test case TC-0020 (comctl-tree-rules).
.DESCRIPTION
Runs the corpus program 224-comctl-tree-rules through wfc
and compares its output. See tests/tc-0020-comctl-tree-rules.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '224-comctl-tree-rules' -Wfc $Wfc
