# Lists what a campaign map contains, using tools/mapinfo. The map is taken from the game's MIX
# archives through the autotest dumpfile step, unless a dump is already in the build's bin folder.
#
#   scripts/mapinfo.ps1 -Map ALL02S.MAP -Build agent26
#   scripts/mapinfo.ps1 -Map ALL02S.MAP -Build agent26 -Find "2 Extra Rocketeers" -Json
#
# -Map is the file name the game finds, such as ALL02S.MAP. The dump is copied to
# <workspace>\build-<Build>\mapinfo\ and read there, so the game's bin folder keeps its own copy.
# When no dump is there, the game runs once through scripts\run_autotest.ps1 with a script that
# only calls dumpfile and quit; it stays hidden, as the runner always does. -Build src uses
# build\src. The workspace is the folder that holds build-<Build>; by default it is the parent of
# this worktree, and -Workspace overrides it.
param(
    [Parameter(Mandatory = $true)][string]$Map,
    [string]$Build = 'src',
    [string]$Find = '',
    [switch]$Json,
    [string]$Workspace = ''
)

$root = Split-Path $PSScriptRoot -Parent
if ($Workspace -eq '') {
    if (Test-Path -LiteralPath (Join-Path $root "build-$Build")) { $Workspace = $root } else { $Workspace = Split-Path $root -Parent }
}
if ($Build -eq 'src') { $buildDir = Join-Path $Workspace 'build\src' } else { $buildDir = Join-Path $Workspace "build-$Build" }
$bin = Join-Path $buildDir 'bin\Release'
$outDir = Join-Path $buildDir 'mapinfo'
$name = Split-Path $Map -Leaf
$dumpName = "dump-$name"
$exe = Join-Path $buildDir 'tools-bin\Release\mapinfo.exe'
if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "mapinfo: $exe is missing; build the mapinfo target in $buildDir first"
    exit 2
}

New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$dest = Join-Path $outDir $dumpName
if (-not (Test-Path -LiteralPath $dest -PathType Leaf)) {
    $dump = Join-Path $bin $dumpName
    if (-not (Test-Path -LiteralPath $dump -PathType Leaf)) {
        $scratch = Join-Path $outDir "dumpfile-$name.txt"
        Set-Content -LiteralPath $scratch -Encoding ascii -Value @("dumpfile $name", 'quit')
        & (Join-Path $Workspace 'scripts\run_autotest.ps1') -Script $scratch -Build $Build -Seconds 60
        if (-not (Test-Path -LiteralPath $dump -PathType Leaf)) {
            Write-Error "mapinfo: the game did not dump $name; check the autotest log in $bin"
            exit 2
        }
    }
    Copy-Item -LiteralPath $dump -Destination $dest
}

$arguments = @()
if ($Json) { $arguments += '--json' }
if ($Find -ne '') { $arguments += @('--find', $Find) }
$arguments += $dest
& $exe @arguments
exit $LASTEXITCODE
