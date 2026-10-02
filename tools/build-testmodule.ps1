[CmdletBinding()]
param([ValidateSet('Build','Publish')][string]$Mode = 'Publish')
$ErrorActionPreference = 'Stop'
$dfExit = 0
try {
    $dfRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
    $dfProject = Join-Path $dfRoot 'testModule\DFTestModule.csproj'
    if (-not (Test-Path -LiteralPath (Join-Path $dfRoot 'global.json'))) {
        throw 'SDK is not pinned. Run tools\pin-dotnet-sdk.ps1 after approval.'
    }
    if (-not (Test-Path -LiteralPath (Join-Path $dfRoot 'testModule\packages.lock.json'))) {
        throw 'NuGet lock is missing. Run tools\restore-testmodule.cmd after approval.'
    }
    . (Join-Path $PSScriptRoot 'DF_DevelopmentSettings.ps1')
    $dfSettings = Get-DFTmSettings -RepositoryRoot $dfRoot
    $dfAssembly = $dfSettings.AssemblyName
    Push-Location -LiteralPath $dfRoot
    try {
        if ($Mode -eq 'Build') {
            & dotnet build $dfProject -c Debug -p:Platform=x64 -p:RestoreLockedMode=true -p:PublishProfile= -p:SelfContained=false -p:PublishSingleFile=false
        } else {
            $dfOutput = Join-Path $dfRoot 'artifacts\publish\TestModule'
            & powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'remove-output-directory.ps1') -Target $dfOutput
            if ($LASTEXITCODE -ne 0) { throw 'Unable to prepare TM publish directory.' }
            $dfPublishDir = $dfOutput.Replace('\','/') + '/'
            & dotnet publish $dfProject -c Release -p:Platform=x64 -p:PublishProfile=FolderProfile -p:RestoreLockedMode=true --self-contained true -p:PublishSingleFile=true "-p:PublishDir=$dfPublishDir"
        }
        $dfExit = $LASTEXITCODE
        if ($dfExit -ne 0) { throw "TM $Mode failed with exit code $dfExit." }
        if ($Mode -eq 'Build') {
            $dfOutput = Join-Path $dfRoot 'bin\testmodule\Debug\win-x64'
            foreach ($dfExtension in @('dll','pdb')) {
                if (-not (Test-Path -LiteralPath (Join-Path $dfOutput ($dfAssembly + '.' + $dfExtension)))) {
                    throw "Missing Debug $dfExtension output."
                }
            }
            & powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'configure-vscode.ps1') -Target TM
            if ($LASTEXITCODE -ne 0) { throw 'VS Code Debug configuration generation failed.' }
        } else {
            $dfPublishedExe = Join-Path $dfOutput ($dfAssembly + '.exe')
            if (-not (Test-Path -LiteralPath $dfPublishedExe)) { throw 'Missing published TM executable.' }
            $dfRelease = Join-Path $dfRoot 'bin\testmodule\Release'
            & powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'remove-output-directory.ps1') -Target $dfRelease
            if ($LASTEXITCODE -ne 0) { throw 'Unable to prepare TM release directory.' }
            New-Item -ItemType Directory -Path $dfRelease -Force | Out-Null
            Copy-Item -LiteralPath $dfPublishedExe -Destination $dfRelease
            $dfOutput = $dfRelease
        }
        Write-Host "[OUTPUT] $dfOutput"
    } finally { Pop-Location }
} catch {
    Write-Error $_
    if ($dfExit -eq 0) { $dfExit = 1 }
}
exit $dfExit
