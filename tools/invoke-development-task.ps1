[CmdletBinding()]
param([Parameter(Mandatory=$true)][ValidateSet('Main','Rod','All','TMDebug','TMPublish')][string]$Task)
$ErrorActionPreference = 'Stop'
$dfScripts = @{ Main='build-main.cmd'; Rod='build-rod.cmd'; All='build-all.cmd'; TMDebug='build-testmodule-debug.cmd'; TMPublish='build-testmodule.cmd' }
$dfArguments = @()
if ($Task -in @('Main','Rod','All')) { $dfArguments = @('Release','x64') }
elseif ($Task -eq 'TMPublish') { $dfArguments = @('Release') }
try {
    & (Join-Path $PSScriptRoot $dfScripts[$Task]) @dfArguments
    exit $LASTEXITCODE
} catch { Write-Error $_; exit 1 }
