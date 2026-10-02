[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
try {
    $dfRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
    $dfSdkList = @(& dotnet --list-sdks)
    if ($LASTEXITCODE -ne 0) { throw 'Unable to list installed .NET SDKs.' }
    $dfVersions = @($dfSdkList | ForEach-Object {
        if ($_ -match '^(8\.0\.[1-4]\d{2})\s') { [version]$matches[1] }
    } | Sort-Object -Descending)
    if ($dfVersions.Count -eq 0) { throw 'Install a stable .NET 8 SDK first.' }
    $dfVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $dfVswhere) {
        $dfVsVersion = & $dfVswhere -latest -products '*' -version '[17.11,18.0)' -property installationVersion
        if ($LASTEXITCODE -ne 0 -or -not $dfVsVersion) { throw 'This environment baseline requires VS2022 17.11 or later.' }
    }
    Write-Host ('[SDK INSTALLED] ' + (($dfVersions | ForEach-Object { $_.ToString() }) -join ', '))
    if ($dfVsVersion) { Write-Host ('[VS2022] ' + $dfVsVersion) }
    $dfFile = Join-Path $dfRoot 'global.json'
    if (Test-Path -LiteralPath $dfFile) { throw 'global.json already exists; update it deliberately rather than replacing it automatically.' }
    $dfConfig = @{ sdk = @{ version = $dfVersions[0].ToString(); rollForward = 'latestPatch'; allowPrerelease = $false } }
    $dfUtf8 = New-Object System.Text.UTF8Encoding($false)
    [IO.File]::WriteAllText($dfFile, (($dfConfig | ConvertTo-Json -Depth 4) + "`r`n"), $dfUtf8)
    Write-Host "[SDK] $($dfVersions[0])"
} catch { Write-Error $_; exit 1 }
