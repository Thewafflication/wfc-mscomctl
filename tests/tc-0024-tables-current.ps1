<#
.SYNOPSIS
Runs test case TC-0024 (member table matches the type-library evidence).
.DESCRIPTION
Compares the SHA-256 hash of the type-library evidence (line endings
normalized to LF) with the first "// SHA256:" line of
src/mscomctl_tables.cpp, as WFC's tests/check_ui_tables.cmake does; see
tc-0024-tables-current.tex. The evidence file is kept in WFC.
.PARAMETER Evidence
Path to evidence/reference/mscomctl-2.0-type-library.json of WFC.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Evidence
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = Split-Path -Parent $PSScriptRoot
$text = (Get-Content -LiteralPath $Evidence -Raw) -replace "`r`n", "`n"
$bytes = [Text.Encoding]::UTF8.GetBytes($text)
$actual = [BitConverter]::ToString(
    [Security.Cryptography.SHA256]::HashData($bytes)).Replace('-', '')
$line = Get-Content -LiteralPath (Join-Path $root 'src/mscomctl_tables.cpp') |
    Where-Object { $_ -match '^// SHA256: ' } | Select-Object -First 1
if (-not $line) {
    throw 'src/mscomctl_tables.cpp has no SHA256 line.'
}
$recorded = $line.Substring('// SHA256: '.Length).Trim()
if ($recorded -ne $actual) {
    throw ('control tables are stale: regenerate with ' +
        'scripts/New-UiControlTables.ps1 in WFC.')
}
Write-Output '[PASS] the member table matches the evidence'
