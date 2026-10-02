[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
try {
    $dfRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).TrimEnd('\')
    $dfMappings = @(& subst.exe)
    if ($LASTEXITCODE -ne 0) { throw 'Unable to read SUBST mappings.' }
    $dfMapping = $dfMappings | Where-Object { $_ -match '^X:\\: => (.+)$' } | Select-Object -First 1
    if ($dfMapping) {
        $dfTarget = [IO.Path]::GetFullPath(($dfMapping -replace '^X:\\: => ', '').Trim()).TrimEnd('\')
        if (-not $dfTarget.Equals($dfRoot, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Drive X: points to a different directory: $dfTarget. Adjust the mapping manually."
        }
    } else {
        if (Test-Path -LiteralPath 'X:\') { throw 'Drive X: is in use. No mapping was changed.' }
        & subst.exe X: $dfRoot
        if ($LASTEXITCODE -ne 0) { throw 'Unable to map this repository to X:.' }
    }
} catch {
    Write-Error $_
    exit 4
}
