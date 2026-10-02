[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$Target)
$ErrorActionPreference = 'Stop'
try {
    $dfRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..')).TrimEnd('\')
    $dfPath = [IO.Path]::GetFullPath($Target).TrimEnd('\')
    $dfPrefix = $dfRoot + '\'
    if (-not $dfPath.StartsWith($dfPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Output path is outside the repository: $dfPath"
    }
    $dfRelative = $dfPath.Substring($dfPrefix.Length)
    $dfAllowed = '^(artifacts\\(build|firmware)\\DF_(Main|Rod)|artifacts\\publish\\TestModule|bin\\(debug|release)\\x64\\V[mr]\d+\.\d+\.\d+\.\d+|bin\\testmodule\\(Debug|Release)(\\win-x64)?)$'
    if ($dfRelative -notmatch $dfAllowed) { throw "Refusing to remove an unexpected output path: $dfPath" }
    $dfCursor = $dfRoot
    foreach ($dfSegment in $dfRelative.Split('\')) {
        $dfCursor = Join-Path $dfCursor $dfSegment
        if (Test-Path -LiteralPath $dfCursor) {
            $dfItem = Get-Item -LiteralPath $dfCursor -Force
            if ($dfItem.Attributes -band [IO.FileAttributes]::ReparsePoint) {
                throw "Refusing to remove output through a junction or symbolic link: $dfCursor"
            }
        }
    }
    if (Test-Path -LiteralPath $dfPath) {
        if (-not (Get-Item -LiteralPath $dfPath -Force).PSIsContainer) { throw 'Output target is not a directory.' }
        # Ancestors and the canonical absolute target have been checked above.
        Remove-Item -LiteralPath $dfPath -Recurse -Force
    }
} catch {
    Write-Error $_
    exit 8
}
