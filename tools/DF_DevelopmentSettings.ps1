function Get-DFTmSettings {
    param([string]$RepositoryRoot)
    [xml]$dfProject = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'testModule\DFTestModule.csproj') -Raw
    $dfVersion = $dfProject.SelectSingleNode('/Project/PropertyGroup/TmVersion').InnerText.Trim()
    $dfAssembly = $dfProject.SelectSingleNode('/Project/PropertyGroup/AssemblyName').InnerText.Trim().Replace('$(TmVersion)', $dfVersion)
    if ($dfVersion -notmatch '^V\d+$' -or $dfAssembly -notmatch '^[A-Za-z0-9_.-]+$') {
        throw 'Unsupported TM version or assembly name in csproj.'
    }
    return @{ Version = $dfVersion; AssemblyName = $dfAssembly }
}
