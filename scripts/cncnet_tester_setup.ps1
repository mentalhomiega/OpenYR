# Sets up a copy of a CnCNet Yuri's Revenge install that starts OpenYR's Game.exe through the
# CnCNet client. The original install is only read. Everything is written into a new folder.
#
# Run it from the folder that holds Game.exe and Language.dll (the unpacked OpenYR zip):
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File cncnet_tester_setup.ps1 -InstallPath "C:\Games\Command & Conquer Red Alert II"
#
# It makes the new folder, copies the install into it, and then changes these files in the new folder only:
#   game.exe                       replaced by Game.exe; the original is kept as game.exe.cncnet-orig
#   Language.dll, ui\              added from the OpenYR build
#   Resources\ClientDefinitions.ini  GameExecutableNames=Game.exe, ExtraCommandLineParams=-SYNCLOG,
#                                    Game.exe added to CompatibilityCheckExecutables; original kept as .cncnet-orig
#   OPENTS.INI                     [Files] Settings=OPENYR.INI, so OpenYR does not share RA2MD.INI with the client
#   updateconfig.ini and Resources\Config\UpdaterConfig_*.ini
#                                  update mirror pointed at 127.0.0.1:9 (nothing listens there); originals kept as .cncnet-orig
#   Resources\ClientDefinitions.ini marked read-only, so the updater cannot write its stock copy back
#   OPENYR-TESTER-COPY.txt         a list of every change, written last
#
# To remove the copy, run it again with -Remove -Destination "<the new folder>". That deletes only
# a folder that holds OPENYR-TESTER-COPY.txt. The install is never changed.
#
# This script never starts the CnCNet client or any program from the install, and it makes no network connection.
param(
	[string]$InstallPath,
	[string]$Destination,
	[string]$BuildPath = $PSScriptRoot,
	[switch]$KeepUpdater,
	[switch]$SkipCompatibilityEntry,
	[switch]$Remove
)

$ErrorActionPreference = 'Stop'
$Marker = 'OPENYR-TESTER-COPY.txt'
$Latin1 = [Text.Encoding]::GetEncoding(28591)   # one char per byte, so untouched bytes round-trip exactly
$Changes = New-Object System.Collections.ArrayList

function Add-Change([string]$Text) {
	[void]$Changes.Add($Text)
	Write-Host $Text
}

function Get-FullPath([string]$Path) {
	return [IO.Path]::GetFullPath($Path).TrimEnd('\')
}

# Replaces a key inside one INI section, or adds the key under the section header when it is missing.
# Returns the old and new line text as a pair, or nothing when the value was already right.
function Set-IniValue([System.Collections.ArrayList]$Lines, [string]$Section, [string]$Key, [string]$Value, [string]$NewLine) {
	$sectionIndex = -1
	for ($i = 0; $i -lt $Lines.Count; $i++) {
		if ($Lines[$i] -match '^\s*\[(.+?)\]\s*$' -and $Matches[1] -ieq $Section) { $sectionIndex = $i; break }
	}
	if ($sectionIndex -lt 0) {
		[void]$Lines.Add("[$Section]")
		[void]$Lines.Add("$Key=$Value")
		return ,@('(section missing)', "$Key=$Value")
	}
	$end = $Lines.Count
	for ($j = $sectionIndex + 1; $j -lt $Lines.Count; $j++) {
		if ($Lines[$j] -match '^\s*\[') { $end = $j; break }
	}
	for ($j = $sectionIndex + 1; $j -lt $end; $j++) {
		if ($Lines[$j] -match ('^\s*' + [regex]::Escape($Key) + '\s*=(.*)$')) {
			$old = $Lines[$j]
			if ($Matches[1].Trim() -ceq $Value) { return }
			$Lines[$j] = "$Key=$Value"
			return ,@($old.Trim(), "$Key=$Value")
		}
	}
	$Lines.Insert($sectionIndex + 1, "$Key=$Value")
	return ,@('(key missing)', "$Key=$Value")
}

function Read-Lines([string]$Path) {
	$text = [IO.File]::ReadAllText($Path, $Latin1)
	$script:NewLineStyle = if ($text.Contains("`r`n")) { "`r`n" } else { "`n" }
	$list = New-Object System.Collections.ArrayList
	$list.AddRange(@($text -split "`r?`n"))
	return ,$list
}

function Write-Lines([string]$Path, [System.Collections.ArrayList]$Lines) {
	[IO.File]::WriteAllText($Path, ($Lines -join $script:NewLineStyle), $Latin1)
}

# ---------------------------------------------------------------- remove
if ($Remove) {
	if (-not $Destination) { throw '-Remove needs -Destination, the copy to delete.' }
	$dest = Get-FullPath $Destination
	$markerPath = Join-Path $dest $Marker
	if (-not (Test-Path -LiteralPath $markerPath)) {
		throw "Refusing to delete '$dest': it has no $Marker, so the script did not create it."
	}
	Get-ChildItem -LiteralPath $dest -Recurse -Force -File | ForEach-Object { $_.IsReadOnly = $false }
	Remove-Item -LiteralPath $dest -Recurse -Force
	Write-Host "Removed the tester copy '$dest'. The original install was not touched."
	return
}

# ---------------------------------------------------------------- checks
if (-not $InstallPath) { throw '-InstallPath is required: the CnCNet Yuri''s Revenge folder.' }
$src = Get-FullPath $InstallPath
$build = Get-FullPath $BuildPath

foreach ($need in @('CnCNetYRLauncher.exe', 'ra2md.mix', 'updateconfig.ini', 'Resources\ClientDefinitions.ini')) {
	if (-not (Test-Path -LiteralPath (Join-Path $src $need))) {
		throw "'$src' does not look like a CnCNet Yuri's Revenge install: $need is missing."
	}
}
if (-not (Test-Path -LiteralPath (Join-Path $build 'Game.exe'))) {
	throw "No Game.exe in '$build'. Run this script from the unpacked OpenYR folder or pass -BuildPath."
}

if (-not $Destination) { $Destination = $src + ' - OpenYR' }
$dst = Get-FullPath $Destination
if ($dst -ieq $src -or $dst.StartsWith($src + '\', [StringComparison]::OrdinalIgnoreCase) -or $dst -ieq $build) {
	throw "The destination must be a new folder outside the install and the build: '$dst'."
}
if (Test-Path -LiteralPath $dst) {
	if (@(Get-ChildItem -LiteralPath $dst -Force).Count -gt 0) {
		throw "The destination '$dst' already exists and is not empty. Pick another folder or run -Remove first."
	}
} elseif (-not (Test-Path -LiteralPath (Split-Path $dst -Parent))) {
	throw "The parent of the destination does not exist: '$dst'."
}

$needBytes = (Get-ChildItem -LiteralPath $src -Recurse -File -Force | Measure-Object -Property Length -Sum).Sum
$freeBytes = (New-Object System.IO.DriveInfo ([IO.Path]::GetPathRoot($dst))).AvailableFreeSpace
if ($freeBytes -lt ($needBytes + 1GB)) {
	throw ("Not enough free space on {0}: the copy needs about {1:N1} GB plus 1 GB." -f [IO.Path]::GetPathRoot($dst), ($needBytes / 1GB))
}

# ---------------------------------------------------------------- copy the install
Write-Host ("Copying the install ({0:N1} GB) to '{1}' ..." -f ($needBytes / 1GB), $dst)
& robocopy.exe $src $dst /E /R:1 /W:1 /NFL /NDL /NJH /NJS /NP | Out-Null
if ($LASTEXITCODE -ge 8) { throw "robocopy failed with exit code $LASTEXITCODE." }
Add-Change "copied    install folder -> $dst (robocopy exit $LASTEXITCODE)"

# ---------------------------------------------------------------- engine files from the build
$gameExe = Join-Path $dst 'Game.exe'
$stockGame = Get-ChildItem -LiteralPath $dst -File | Where-Object { $_.Name -ieq 'game.exe' } | Select-Object -First 1
if ($stockGame) {
	Copy-Item -LiteralPath $stockGame.FullName -Destination (Join-Path $dst 'game.exe.cncnet-orig') -Force
	Add-Change "backup    $($stockGame.Name) -> game.exe.cncnet-orig (the stock file, kept)"
}
Copy-Item -LiteralPath (Join-Path $build 'Game.exe') -Destination $gameExe -Force
Add-Change "replaced  Game.exe ($((Get-Item -LiteralPath $gameExe).Length) bytes, from $build)"

if (Test-Path -LiteralPath (Join-Path $build 'Language.dll')) {
	Copy-Item -LiteralPath (Join-Path $build 'Language.dll') -Destination (Join-Path $dst 'Language.dll') -Force
	Add-Change 'added     Language.dll'
}
if (Test-Path -LiteralPath (Join-Path $build 'ui')) {
	Get-ChildItem -LiteralPath (Join-Path $build 'ui') -Recurse -File | ForEach-Object {
		$rel = $_.FullName.Substring($build.Length + 1)
		$target = Join-Path $dst $rel
		New-Item -ItemType Directory -Force -Path (Split-Path $target -Parent) | Out-Null
		Copy-Item -LiteralPath $_.FullName -Destination $target -Force
		Add-Change "added     $rel"
	}
}

# ---------------------------------------------------------------- ClientDefinitions.ini
$defs = Join-Path $dst 'Resources\ClientDefinitions.ini'
Copy-Item -LiteralPath $defs -Destination ($defs + '.cncnet-orig') -Force
Add-Change 'backup    Resources\ClientDefinitions.ini -> Resources\ClientDefinitions.ini.cncnet-orig'
$defsLines = Read-Lines $defs
$edits = @()
$edits += ,@('GameExecutableNames', (Set-IniValue $defsLines 'Settings' 'GameExecutableNames' 'Game.exe' $script:NewLineStyle))
$edits += ,@('ExtraCommandLineParams', (Set-IniValue $defsLines 'Settings' 'ExtraCommandLineParams' '-SYNCLOG' $script:NewLineStyle))
if ($SkipCompatibilityEntry) {
	Add-Change 'skipped   CompatibilityCheckExecutables (-SkipCompatibilityEntry): Game.exe is not added'
} else {
	$compat = $null
	for ($i = 0; $i -lt $defsLines.Count; $i++) {
		if ($defsLines[$i] -match '^\s*CompatibilityCheckExecutables\s*=(.*)$') { $compat = $Matches[1].Trim() }
	}
	if ($compat -and -not (($compat -split ',') | Where-Object { $_.Trim() -ieq 'Game.exe' })) {
		$edits += ,@('CompatibilityCheckExecutables', (Set-IniValue $defsLines 'Settings' 'CompatibilityCheckExecutables' ($compat + ',Game.exe') $script:NewLineStyle))
	}
}
Write-Lines $defs $defsLines
foreach ($edit in $edits) {
	if ($edit[1]) { Add-Change ("edited    Resources\ClientDefinitions.ini [Settings] {0}: {1} -> {2}" -f $edit[0], $edit[1][0], $edit[1][1]) }
}

# ---------------------------------------------------------------- OPENTS.INI: keep our settings out of RA2MD.INI
$opents = Join-Path $dst 'OPENTS.INI'
if (Test-Path -LiteralPath $opents) {
	Add-Change 'skipped   OPENTS.INI already exists in the copy; it was left as it is'
} else {
	[IO.File]::WriteAllText($opents, "[Files]$($script:NewLineStyle)Settings=OPENYR.INI$($script:NewLineStyle)", $Latin1)
	Add-Change 'added     OPENTS.INI ([Files] Settings=OPENYR.INI, so OpenYR keeps its settings apart from the client''s RA2MD.INI)'
}

# ---------------------------------------------------------------- updater
if ($KeepUpdater) {
	Add-Change 'skipped   updater (-KeepUpdater): the update mirrors and ClientDefinitions.ini were left as they are'
} else {
	# The client copies the chosen Resources\Config\UpdaterConfig_<channel>.ini over updateconfig.ini
	# at start (OptionsWindow.ini, OverwriteOnMismatch), so every channel file gets the same mirror.
	$updFiles = @(Join-Path $dst 'updateconfig.ini') + @(Get-ChildItem -LiteralPath (Join-Path $dst 'Resources\Config') -Filter 'UpdaterConfig_*.ini' | ForEach-Object { $_.FullName })
	foreach ($upd in $updFiles) {
		$name = $upd.Substring($dst.Length + 1)
		Copy-Item -LiteralPath $upd -Destination ($upd + '.cncnet-orig') -Force
		Add-Change "backup    $name -> $name.cncnet-orig"
		$updLines = Read-Lines $upd
		$kept = New-Object System.Collections.ArrayList
		foreach ($line in $updLines) {
			if ($line.Trim() -eq '' -or $line.TrimStart().StartsWith(';')) { [void]$kept.Add($line) }
		}
		[void]$kept.Add('https://127.0.0.1:9/, OpenYR test copy (updates off), Local')
		[void]$kept.Add('')   # ends the file with a line break
		Write-Lines $upd $kept
		Add-Change "edited    ${name}: the update mirror is 127.0.0.1:9, where nothing listens"
	}
	(Get-Item -LiteralPath $defs).IsReadOnly = $true
	Add-Change 'readonly  Resources\ClientDefinitions.ini (the updater cannot write its stock copy over it)'
}

# ---------------------------------------------------------------- marker
$report = @(
	'OpenYR tester copy of a CnCNet Yuri''s Revenge install',
	"Created:  $(Get-Date -Format 's')",
	"Install:  $src (not changed)",
	"Build:    $build",
	'',
	'Changes made in this copy:'
) + ($Changes | ForEach-Object { '  ' + $_ }) + @(
	'',
	'Undo: run cncnet_tester_setup.ps1 -Remove -Destination with this folder. That deletes only this copy.'
)
[IO.File]::WriteAllText((Join-Path $dst $Marker), ($report -join "`r`n") + "`r`n", $Latin1)

Write-Host ''
Write-Host "Done. Start the CnCNet client from: $dst"
Write-Host 'Set the client to the copy folder before you host or join. The original install is unchanged.'
