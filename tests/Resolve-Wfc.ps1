<#
.SYNOPSIS
Finds the wfc executable that runs the corpus programs and the example.
.DESCRIPTION
Uses the path given, else the WFC_EXE environment variable, else wfc on
PATH. Throws when none is found: the corpus programs are Visual Basic 6
programs that only WFC runs.
.PARAMETER Wfc
An explicit path to wfc.exe.
.OUTPUTS
System.String. The full path of wfc.exe.
#>
[CmdletBinding()]
[OutputType([string])]
param(
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

if (-not $Wfc) {
    $Wfc = $env:WFC_EXE
}
if (-not $Wfc) {
    $found = Get-Command wfc -CommandType Application `
        -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($found) {
        $Wfc = $found.Source
    }
}
if (-not $Wfc -or -not (Test-Path -LiteralPath $Wfc -PathType Leaf)) {
    throw ('wfc.exe not found: pass -Wfc, set WFC_EXE or put wfc on ' +
        'PATH (WFC runs the corpus programs).')
}
return (Resolve-Path -LiteralPath $Wfc).Path
