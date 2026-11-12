# Makes a mod folder for Mental Omega from an installed copy of the mod. The install is only read.
#
#   powershell.exe -NoProfile -File scripts\setup_mo_mod.ps1 -Source "D:\Games\Mental Omega" -Dest "<DataDir>\Mods\MentalOmega"
#
# It copies the mod's data files and writes a mod.ini that names them:
#   expandmo*.mix, ecachemo*.mix, thememo.mix   the mod's archives (-Link links them instead of copying)
#   rules, art, AI, sound, eva, theme and battle INI files, ra2md.csf and stringtable*.csf, when they sit loose in the install
#   with -Maps, the .map files under MapsMO, flat into the folder, so a skirmish can name them
#   the client's game modes and game options: INI\MentalOmegaMaps.ini, INI\Map Code\*.ini, INI\Game Options\*.ini and
#   Resources\GameOptions.ini, kept in their folders, and a [Lobby] section in mod.ini that names them (docs/LOBBY_MODES.md)
# The mod's uimd.ini hides the Single Player and Skirmish buttons, which suits the launcher the mod is played through, so mod.ini
# sets FollowMenuButtons=no to keep them. Remove that line to follow the mod's menu.
# It never copies a DLL, an EXE, a save or any client file but the lobby files above. The base game's archives (ra2md.mix, langmd.mix and so on)
# stay in the data directory; the mod needs its own copy of the game, as Mental Omega does.
#
# The mod is then chosen with Mods=MentalOmega in [Options] of RA2MD.INI, with -MOD=MentalOmega, or on the Mods screen,
# and takes effect at the next start.
param(
	[Parameter(Mandatory = $true)][string]$Source,
	[Parameter(Mandatory = $true)][string]$Dest,
	[switch]$Link,
	[switch]$Maps
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path -LiteralPath $Source -PathType Container)) {
	Write-Error "setup_mo_mod: no folder $Source"
	exit 1
}
$sourceFull = (Resolve-Path -LiteralPath $Source).Path
New-Item -ItemType Directory -Force -Path $Dest | Out-Null
$destFull = (Resolve-Path -LiteralPath $Dest).Path

# Only data files of these kinds are taken, so that no program of the install can be copied by mistake.
$wanted = @(
	'^expandmo\d\d\.mix$', '^ecachemo\d\d\.mix$', '^thememo\.mix$',
	'^(rules|art|ai|sound|eva|theme|battle)mo\.ini$',
	'^ra2md\.csf$', '^stringtable\d\d\.csf$'
)

$copied = 0
$kept = 0
function Install-File([System.IO.FileInfo]$file, [string]$target) {
	if ((Test-Path -LiteralPath $target) -and ((Get-Item -LiteralPath $target).Length -eq $file.Length)) {
		$script:kept++
		return
	}
	if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Force }
	if ($Link) {
		New-Item -ItemType HardLink -Path $target -Target $file.FullName | Out-Null
	} else {
		Copy-Item -LiteralPath $file.FullName -Destination $target
	}
	$script:copied++
}

foreach ($file in Get-ChildItem -LiteralPath $sourceFull -File) {
	foreach ($pattern in $wanted) {
		if ($file.Name -match $pattern) {
			Install-File $file (Join-Path $destFull $file.Name)
			break
		}
	}
}

# The lobby files keep their folders, because the client's files name one another by those paths.
$lobby = @('INI\MentalOmegaMaps.ini', 'Resources\GameOptions.ini')
foreach ($folder in @('INI\Map Code', 'INI\Game Options')) {
	$full = Join-Path $sourceFull $folder
	if (Test-Path -LiteralPath $full -PathType Container) {
		$lobby += @(Get-ChildItem -LiteralPath $full -File -Filter *.ini | ForEach-Object { Join-Path $folder $_.Name })
	}
}
$lobbyFound = $false
foreach ($relative in $lobby) {
	$file = Join-Path $sourceFull $relative
	if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { continue }
	$target = Join-Path $destFull $relative
	New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
	Install-File (Get-Item -LiteralPath $file) $target
	if ($relative -eq 'INI\MentalOmegaMaps.ini') { $lobbyFound = $true }
}

# The modes the client also offers for maps its list does not name.
$customModes = ''
$definitions = Join-Path $sourceFull 'Resources\ClientDefinitions.ini'
if (Test-Path -LiteralPath $definitions -PathType Leaf) {
	$line = Select-String -LiteralPath $definitions -Pattern '^\s*AllowedCustomGameModes\s*=\s*(.*?)\s*$' | Select-Object -First 1
	if ($line) { $customModes = $line.Matches[0].Groups[1].Value }
}

if ($Maps) {
	$mapRoot = Join-Path $sourceFull 'MapsMO'
	if (Test-Path -LiteralPath $mapRoot -PathType Container) {
		foreach ($file in Get-ChildItem -LiteralPath $mapRoot -Recurse -File -Filter *.map) {
			Install-File $file (Join-Path $destFull $file.Name)
		}
	}
}

$manifest = @"
[Mod]
Name=Mental Omega
Description=Mental Omega, an Ares based Yuri's Revenge mod. Needs the game's own data in the data directory.

[Files]
Rules=RULESMO.INI
Art=ARTMO.INI
AI=AIMO.INI
Sound=SOUNDMO.INI
Theme=THEMEMO.INI
Eva=EVAMO.INI
Battle=BATTLEMO.INI
TheaterSuffix=MO
Mission=MISSIONMO.INI
MapSelect=MAPSELMO.INI

[Archives]
ExpandPrefix=EXPANDMO,EXPANDMD

[Menu]
FollowMenuButtons=no
"@
if ($lobbyFound) {
	$manifest += @"


[Lobby]
Maps=INI\MentalOmegaMaps.ini
Options=Resources\GameOptions.ini
CustomMapModes=$customModes
"@
}
Set-Content -LiteralPath (Join-Path $destFull 'mod.ini') -Value $manifest -Encoding ascii

"setup_mo_mod: $copied files written, $kept already there, mod.ini written in $destFull"
$present = @(Get-ChildItem -LiteralPath $destFull -File -Filter 'expandmo*.mix').Count
if ($present -eq 0) { Write-Warning 'no expandmo*.mix found in the source; the mod will have no rules' }
