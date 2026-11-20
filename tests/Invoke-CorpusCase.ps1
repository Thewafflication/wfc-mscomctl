<#
.SYNOPSIS
Runs one corpus program through wfc and compares its output.
.DESCRIPTION
Runs "wfc --interpret <case>.vbp" and compares standard output with
<case>.expected and the Immediate-window text on standard error with
<case>.debug, after normalizing line endings and surrounding blank space,
as WFC's corpus runner does. Throws on the first difference.
.PARAMETER Case
The file name of the case without extension, for example
188-comctl-basic.
.PARAMETER Wfc
An explicit path to wfc.exe (see Resolve-Wfc.ps1).
.EXAMPLE
pwsh -NoProfile -File tests/tc-0010-comctl-basic.ps1
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Case,
    [string]$Wfc
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function ConvertTo-Comparable {
    param([AllowNull()][string]$Text)

    if ($null -eq $Text) {
        return ''
    }
    return ($Text -replace "`r`n", "`n").Trim()
}

$wfcPath = & (Join-Path $PSScriptRoot 'Resolve-Wfc.ps1') -Wfc $Wfc
$directory = Join-Path $PSScriptRoot 'corpus'
$project = Join-Path $directory "$Case.vbp"
$errorFile = New-TemporaryFile
try {
    $output = (& $wfcPath --interpret $project 2> $errorFile) -join "`n"
    if ($LASTEXITCODE -ne 0) {
        throw "wfc exited with $LASTEXITCODE for $Case."
    }
    $actualOutput = ConvertTo-Comparable $output
    $actualDebug = ConvertTo-Comparable (Get-Content -LiteralPath $errorFile `
            -Raw)
    $expectedOutput = ConvertTo-Comparable (Get-Content `
            -LiteralPath (Join-Path $directory "$Case.expected") -Raw)
    $debugFile = Join-Path $directory "$Case.debug"
    $expectedDebug = ''
    if (Test-Path -LiteralPath $debugFile) {
        $expectedDebug = ConvertTo-Comparable (Get-Content `
                -LiteralPath $debugFile -Raw)
    }
    if ($actualDebug -ne $expectedDebug) {
        throw ("Immediate-window output of $Case differs.`n" +
            "--- expected ---`n$expectedDebug`n--- actual ---`n$actualDebug")
    }
    if ($actualOutput -ne $expectedOutput) {
        throw ("Output of $Case differs.`n" +
            "--- expected ---`n$expectedOutput`n--- actual ---`n" +
            $actualOutput)
    }
    Write-Output "[PASS] $Case"
}
finally {
    Remove-Item -LiteralPath $errorFile -ErrorAction SilentlyContinue
}
