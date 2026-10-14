# Sets up a copy of a CnCNet Yuri's Revenge install that starts OpenYR's Game.exe through the
# CnCNet client. The original install is only read. Everything is written into a new folder.
#
# Double-click "Set up CnCNet.bat" beside Game.exe. With no -InstallPath the script finds the install
# or asks for it, shows the copy folder, and asks before copying anything.
# From a prompt, run it from the folder that holds Game.exe and Language.dll (the unpacked OpenYR zip):
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File cncnet_tester_setup.ps1 -InstallPath "C:\Games\Command & Conquer Red Alert II"
# Without -Destination the copy goes beside the install as "<install> - OpenYR" when that folder is
# writable, and otherwise to "<system drive>\Games\OpenYR CnCNet".
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
# To remove the copy, double-click "Remove CnCNet copy.bat", or run the script with -Remove
# -Destination "<the new folder>". That deletes only a folder that holds OPENYR-TESTER-COPY.txt.
# The install is never changed.
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

function Test-Inside([string]$Path, [string]$Folder) {
	return $Path -ieq $Folder -or $Path.StartsWith($Folder + '\', [StringComparison]::OrdinalIgnoreCase)
}

function Test-CnCNetInstall([string]$Path) {
	return $Path -and (Test-Path -LiteralPath (Join-Path $Path 'CnCNetYRLauncher.exe')) -and (Test-Path -LiteralPath (Join-Path $Path 'Resources\ClientDefinitions.ini'))
}

# Returns the OpenYR folder when it is a CnCNet install (testers often unpack OpenYR there), or else the
# first CnCNet install named by the game's registry keys or found in the usual store folders.
function Find-CnCNetInstall {
	$candidates = New-Object System.Collections.ArrayList
	[void]$candidates.Add($BuildPath)
	foreach ($key in @('HKLM:\SOFTWARE\WOW6432Node\Westwood\Yuri''s Revenge', 'HKLM:\SOFTWARE\Westwood\Yuri''s Revenge', 'HKCU:\SOFTWARE\Westwood\Yuri''s Revenge')) {
		$value = (Get-ItemProperty -LiteralPath $key -Name InstallPath -ErrorAction SilentlyContinue).InstallPath
		if ($value) { [void]$candidates.Add($(if ($value -match '\.exe$') { Split-Path $value -Parent } else { $value })) }
	}
	foreach ($base in @(${env:ProgramFiles(x86)}, $env:ProgramFiles)) {
		if ($base) {
			[void]$candidates.Add((Join-Path $base 'Steam\steamapps\common\Command & Conquer Red Alert II'))
			[void]$candidates.Add((Join-Path $base 'Origin Games\Command and Conquer Red Alert II'))
			[void]$candidates.Add((Join-Path $base 'EA Games\Command and Conquer Red Alert II'))
		}
	}
	foreach ($folder in $candidates) {
		if (Test-CnCNetInstall $folder) { return (Get-FullPath $folder) }
	}
	return $null
}

function Test-Writable([string]$Folder) {
	if (-not (Test-Path -LiteralPath $Folder)) { return $false }
	$probe = Join-Path $Folder ('openyr-write-test-' + [guid]::NewGuid().ToString('N') + '.tmp')
	try {
		[IO.File]::WriteAllText($probe, '')
		Remove-Item -LiteralPath $probe -Force
		return $true
	} catch {
		return $false
	}
}

function Get-DefaultDestination([string]$Install) {
	if (Test-Writable (Split-Path $Install -Parent)) { return $Install + ' - OpenYR' }
	return Join-Path ([IO.Path]::GetPathRoot($env:SystemRoot)) 'Games\OpenYR CnCNet'
}

function Confirm-Step([string]$Question) {
	while ($true) {
		$answer = Read-Host "$Question [Y/N]"
		if ($answer -match '^\s*y') { return $true }
		if ($answer -match '^\s*n') { return $false }
	}
}

function Select-Folder([string]$Prompt) {
	Add-Type -AssemblyName System.Windows.Forms
	$owner = New-Object System.Windows.Forms.Form -Property @{ TopMost = $true }
	$dialog = New-Object System.Windows.Forms.FolderBrowserDialog
	$dialog.Description = $Prompt
	$dialog.ShowNewFolderButton = $true
	try {
		if ($dialog.ShowDialog($owner) -ne [System.Windows.Forms.DialogResult]::OK) { throw 'No folder was chosen, so nothing was changed.' }
		return $dialog.SelectedPath
	} finally {
		$owner.Dispose()
	}
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
	$ask = -not $Destination
	if ($ask) { $Destination = Select-Folder "Choose the OpenYR copy to remove: the folder that holds $Marker." }
	$dest = Get-FullPath $Destination
	$markerPath = Join-Path $dest $Marker
	if (-not (Test-Path -LiteralPath $markerPath)) {
		throw "Refusing to delete '$dest': it has no $Marker, so the script did not create it."
	}
	if ($ask -and -not (Confirm-Step "Delete the OpenYR copy '$dest' and everything in it?")) {
		Write-Host 'Nothing was deleted.'
		return
	}
	Get-ChildItem -LiteralPath $dest -Recurse -Force -File | ForEach-Object { $_.IsReadOnly = $false }
	Remove-Item -LiteralPath $dest -Recurse -Force
	Write-Host "Removed the tester copy '$dest'. The original install was not touched."
	return
}

# ---------------------------------------------------------------- checks
$Interactive = -not $InstallPath
if ($Interactive) {
	Write-Host 'OpenYR CnCNet setup: makes a separate copy of your CnCNet Yuri''s Revenge install that starts OpenYR.'
	Write-Host 'Your install is only read. Nothing is copied or changed until you confirm.'
	Write-Host ''
	$InstallPath = Find-CnCNetInstall
	if ($InstallPath) {
		Write-Host "Found a CnCNet install: $InstallPath"
		if (-not (Confirm-Step 'Use this install?')) { $InstallPath = $null }
	}
	if (-not $InstallPath) { $InstallPath = Select-Folder 'Choose your CnCNet Yuri''s Revenge folder: the one that holds CnCNetYRLauncher.exe.' }
}
$src = Get-FullPath $InstallPath
$build = Get-FullPath $BuildPath
if ($Interactive -and -not $Destination) {
	$Destination = Get-DefaultDestination $src
	Write-Host "The OpenYR copy will be made in: $Destination"
	if (-not (Confirm-Step 'Use this folder?')) {
		$Destination = Join-Path (Select-Folder 'Choose where to put the OpenYR copy. A new folder named "OpenYR CnCNet" is made inside the one you choose.') 'OpenYR CnCNet'
	}
}

foreach ($need in @('CnCNetYRLauncher.exe', 'ra2md.mix', 'updateconfig.ini', 'Resources\ClientDefinitions.ini')) {
	if (-not (Test-Path -LiteralPath (Join-Path $src $need))) {
		throw "'$src' does not look like a CnCNet Yuri's Revenge install: $need is missing."
	}
}
if (-not (Test-Path -LiteralPath (Join-Path $build 'Game.exe'))) {
	throw "No Game.exe in '$build'. Run this script from the unpacked OpenYR folder or pass -BuildPath."
}

if (-not $Destination) { $Destination = Get-DefaultDestination $src }
$dst = Get-FullPath $Destination
if ((Test-Inside $dst $src) -or (Test-Inside $src $dst) -or (Test-Inside $build $dst)) {
	throw "The copy must go in a new folder outside the install and outside the OpenYR folder: '$dst'."
}
if (Test-Path -LiteralPath $dst) {
	if (@(Get-ChildItem -LiteralPath $dst -Force).Count -gt 0) {
		throw "The destination '$dst' already exists and is not empty. Pick another folder or run -Remove first."
	}
} elseif (-not (Test-Path -LiteralPath (Split-Path $dst -Parent))) {
	if (-not $Interactive) { throw "The parent of the destination does not exist: '$dst'." }
	New-Item -ItemType Directory -Force -Path (Split-Path $dst -Parent) | Out-Null
}
if (-not (Test-Writable (Split-Path $dst -Parent))) {
	throw "Windows does not allow writing to '$(Split-Path $dst -Parent)' without administrator rights. Choose another folder."
}

$needBytes = (Get-ChildItem -LiteralPath $src -Recurse -File -Force | Measure-Object -Property Length -Sum).Sum
$freeBytes = (New-Object System.IO.DriveInfo ([IO.Path]::GetPathRoot($dst))).AvailableFreeSpace
if ($freeBytes -lt ($needBytes + 1GB)) {
	throw ("Not enough free space on {0}: the copy needs about {1:N1} GB plus 1 GB." -f [IO.Path]::GetPathRoot($dst), ($needBytes / 1GB))
}
if ($Interactive) {
	Write-Host ''
	Write-Host 'Ready to:'
	Write-Host ("  copy {0}" -f $src)
	Write-Host ("  to   {0} (about {1:N1} GB)" -f $dst, ($needBytes / 1GB))
	Write-Host "  put OpenYR's Game.exe, Language.dll and ui folder from $build into the copy"
	Write-Host '  set the copy''s CnCNet client to start Game.exe, and turn off its updater'
	Write-Host "Every change is printed, and listed afterwards in $Marker in the copy."
	if (-not (Confirm-Step 'Go ahead?')) {
		Write-Host 'Nothing was copied or changed.'
		return
	}
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
Write-Host "Done. The OpenYR copy is in $dst, and the original install is unchanged."
Write-Host 'Start the CnCNet client from the copy, never from the original install, when you play OpenYR.'
if ($Interactive) {
	$launcher = Join-Path $dst 'CnCNetYRLauncher.exe'
	if (Confirm-Step 'Put a shortcut named "OpenYR CnCNet" on your desktop?') {
		$shell = New-Object -ComObject WScript.Shell
		$link = $shell.CreateShortcut((Join-Path ([Environment]::GetFolderPath('Desktop')) 'OpenYR CnCNet.lnk'))
		$link.TargetPath = $launcher
		$link.WorkingDirectory = $dst
		$link.Save()
		Write-Host 'Added the desktop shortcut "OpenYR CnCNet". Start CnCNet with it.'
	} else {
		Write-Host "Start CnCNet with CnCNetYRLauncher.exe in $dst."
	}
}
