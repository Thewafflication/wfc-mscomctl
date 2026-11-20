<#
.SYNOPSIS
Runs test case TC-0013 (comctl-treeview).
.DESCRIPTION
Runs the corpus program 191-comctl-treeview through wfc
and compares its output. See tests/tc-0013-comctl-treeview.tex.
.PARAMETER Wfc
An explicit path to wfc.exe (else WFC_EXE, else wfc on PATH).
#>
[CmdletBinding()]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'Invoke-CorpusCase.ps1') `
    -Case '191-comctl-treeview' -Wfc $Wfc
