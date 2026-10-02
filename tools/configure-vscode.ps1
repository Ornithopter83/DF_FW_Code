[CmdletBinding()]
param([ValidateSet('All','FW','TM')][string]$Target = 'All')
$ErrorActionPreference = 'Stop'
$dfRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$dfUtf8 = New-Object System.Text.UTF8Encoding($false)
function Write-DFJson {
    param([string]$Path, $Value)
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($Path)) | Out-Null
    [IO.File]::WriteAllText($Path, (($Value | ConvertTo-Json -Depth 30) + "`r`n"), $dfUtf8)
}
try {
    if ($Target -in @('All','TM')) {
        . (Join-Path $PSScriptRoot 'DF_DevelopmentSettings.ps1')
        $dfTm = Get-DFTmSettings -RepositoryRoot $dfRoot
        Write-DFJson (Join-Path $dfRoot 'testModule\.vscode\launch.json') @{
            version = '0.2.0'; configurations = @(@{
                name = 'TM Debug (shared project)'; type = 'coreclr'; request = 'launch'
                preLaunchTask = 'TM: Debug build'
                program = '${workspaceFolder}/../bin/testmodule/Debug/win-x64/' + $dfTm.AssemblyName + '.dll'
                args = @(); cwd = '${workspaceFolder}/..'; stopAtEntry = $false
                console = 'internalConsole'; justMyCode = $true
            })
        }
    }
    if ($Target -in @('All','FW')) {
        [xml]$dfProps = Get-Content -LiteralPath (Join-Path $dfRoot 'vs\DF_Arduino_ESP32S3.props') -Raw
        $dfNs = New-Object Xml.XmlNamespaceManager($dfProps.NameTable)
        $dfNs.AddNamespace('m','http://schemas.microsoft.com/developer/msbuild/2003')
        $dfReadProperty = { param($Name) $dfProps.SelectSingleNode('//m:' + $Name, $dfNs).InnerText }
        $dfVs = (Join-Path $dfRoot 'vs') + '\'
        $dfPlatform = [IO.Path]::GetFullPath((& $dfReadProperty 'DFArduinoPlatformRoot').Replace('$(MSBuildThisFileDirectory)', $dfVs))
        $dfIncludes = @((& $dfReadProperty 'DFArduinoIncludeSearchPath').Split(';') | ForEach-Object {
            [IO.Path]::GetFullPath($_.Replace('$(DFArduinoPlatformRoot)', $dfPlatform)).Replace('\','/')
        })
        $dfDefines = @((& $dfReadProperty 'DFArduinoPreprocessorDefinitions').Split(';'))
        $dfCompilerFolder = Join-Path $dfRoot 'toolchain\arduino-data\packages\esp32\tools\xtensa-esp32s3-elf-gcc'
        $dfCompilers = @(Get-ChildItem -LiteralPath $dfCompilerFolder -Filter 'xtensa-esp32s3-elf-g++.exe' -Recurse -File -ErrorAction SilentlyContinue | Sort-Object FullName)
        $dfConfigurations = @()
        foreach ($dfFirmware in @('DF_Main','DF_Rod')) {
            $dfConfig = @{
                name = $dfFirmware
                includePath = @(('${workspaceFolder}/firmware/' + $dfFirmware + '/**'), '${workspaceFolder}/libraries/DFProtocol/src') + $dfIncludes
                defines = $dfDefines; cppStandard = 'gnu++11'; cStandard = 'gnu99'
                intelliSenseMode = 'windows-gcc-x64'
                forcedInclude = @($dfPlatform.Replace('\','/') + '/cores/esp32/Arduino.h')
            }
            if ($dfCompilers.Count -gt 0) { $dfConfig.compilerPath = $dfCompilers[0].FullName.Replace('\','/') }
            $dfDatabase = Join-Path $dfRoot ('artifacts\build\' + $dfFirmware + '\compile_commands.json')
            if (Test-Path -LiteralPath $dfDatabase) {
                $dfCommands = @(Get-Content -LiteralPath $dfDatabase -Raw | ConvertFrom-Json)
                $dfNormalized = @()
                foreach ($dfEntry in $dfCommands) {
                    $dfFile = ([string]$dfEntry.file).Replace('\','/')
                    $dfOldRoot = $null
                    if ($dfFile -match '^(.*?)/(artifacts|toolchain|firmware|libraries)/') { $dfOldRoot = $matches[1] }
                    elseif ($dfFile -match '^X:/') { $dfOldRoot = 'X:' }
                    $dfNewRoot = $dfRoot.Replace('\','/')
                    $dfTranslate = {
                        param([string]$Text)
                        if ($Text.StartsWith('-D')) { return $Text }
                        $dfValue = $Text.Replace('\"','__DF_ESCAPED_QUOTE__').Replace('\','/')
                        if ($dfOldRoot) { $dfValue = $dfValue.Replace($dfOldRoot, $dfNewRoot) }
                        $dfValue = $dfValue.Replace('/artifacts/build/' + $dfFirmware + '/sketch/src/', '/firmware/' + $dfFirmware + '/src/')
                        $dfValue = $dfValue.Replace('/artifacts/build/' + $dfFirmware + '/sketch/' + $dfFirmware + '.ino.cpp', '/firmware/' + $dfFirmware + '/' + $dfFirmware + '.ino')
                        $dfValue = $dfValue.Replace('X:/', $dfNewRoot + '/')
                        return $dfValue.Replace('__DF_ESCAPED_QUOTE__','\"')
                    }
                    $dfRow = @{ directory = (& $dfTranslate ([string]$dfEntry.directory)); file = (& $dfTranslate $dfFile) }
                    if ($dfEntry.PSObject.Properties['arguments']) {
                        $dfRow.arguments = @($dfEntry.arguments | ForEach-Object { & $dfTranslate ([string]$_) })
                    } else { $dfRow.command = & $dfTranslate ([string]$dfEntry.command) }
                    $dfNormalized += $dfRow
                }
                Write-DFJson (Join-Path $dfRoot ('artifacts\vscode\' + $dfFirmware + '_compile_commands.json')) $dfNormalized
                $dfConfig.compileCommands = '${workspaceFolder}/artifacts/vscode/' + $dfFirmware + '_compile_commands.json'
            }
            $dfConfigurations += $dfConfig
        }
        Write-DFJson (Join-Path $dfRoot '.vscode\c_cpp_properties.json') @{ version = 4; configurations = $dfConfigurations }
    }
    Write-Host '[CONFIG] VS Code configuration written. No build or SDK installation was run.'
} catch { Write-Error $_; exit 1 }
