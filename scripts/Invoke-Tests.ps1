<#
.SYNOPSIS
Runs the library CTest suite (x64 debug) and logs it for the progress window.
.DESCRIPTION
Runs every registered CTest test in out/build; zero registered tests is an
error. The build must already exist (scripts/Invoke-Build.ps1). Each line
ctest prints (such as `12/377 Test  #12: name ... Passed`) is also appended
to out/test-progress.log, which scripts/Show-TestProgress.ps1 follows.
.PARAMETER JUnitPath
Optional path for a JUnit XML result file (used by CI evidence).
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Tests.ps1
#>
[CmdletBinding()]
param(
    [string]$JUnitPath = ''
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    # The log is followed live by scripts/Show-TestProgress.ps1.
    $progressLog = Join-Path $root 'out/test-progress.log'
    New-Item -ItemType Directory -Force -Path (Split-Path $progressLog) |
        Out-Null
    $arguments = @('--test-dir', 'out/build', '-C', 'Debug',
        '--output-on-failure', '--no-tests=error')
    if ($JUnitPath) {
        $arguments += @('--output-junit', $JUnitPath)
    }
    [IO.File]::WriteAllText($progressLog, '')
    # Each line goes to the log as soon as ctest prints it.
    & ctest @arguments 2>&1 | ForEach-Object {
        $line = "$_"
        [IO.File]::AppendAllText($progressLog, $line + "`n")
        $line
    }
    if ($LASTEXITCODE -ne 0) {
        throw "ctest $($arguments -join ' ') failed ($LASTEXITCODE)."
    }
    Write-Output '[PASS] tests x64 Debug'
}
finally {
    Pop-Location
}
