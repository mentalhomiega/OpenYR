# Makes the scratch folders the CnCNet page screenshots run against. Nothing here touches a real CnCNet
# install: the install is a copy of the text files of a client folder, its executables are placeholder
# bytes that are never run, and every folder is under -Root.
#
#   -Install   writes the fake install under -Root\install, from -Source
#   -Edit      puts the stock ClientDefinitions.ini back in the copy, as a client update does
#   -Lock      makes -Root\locked a folder this user cannot write to
#   -Unlock    gives the write permission back and removes the locked folder
#   -Settings  prints the [CnCNet] section the page reads, for run_autotest.ps1 -Settings
#
# Example, from the workspace root:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File src-agent16/tests/autotest/cncnet-page-fixture.ps1 -Install
param(
	[string]$Root = (Join-Path $env:TEMP 'openyr-cncnet-page'),
	[string]$Source = 'E:\Claude ra2 decomp\cncnet-files\client',
	[switch]$Install, [switch]$Edit, [switch]$Lock, [switch]$Unlock, [switch]$Settings,
	[string]$Destination = ''
)

$installDir = Join-Path $Root 'install'
$copyDir = Join-Path $Root 'OpenYR copy'
$lockedDir = Join-Path $Root 'locked'
$userName = $env:USERNAME

if ($Install) {
	if (-not (Test-Path -LiteralPath (Join-Path $Source 'Resources\ClientDefinitions.ini'))) {
		Write-Error "fixture: $Source has no Resources\ClientDefinitions.ini"
		exit 1
	}
	Remove-Item -LiteralPath $installDir -Recurse -Force -ErrorAction SilentlyContinue
	New-Item -ItemType Directory -Force $installDir | Out-Null
	# The client folder holds text files only: INI, JSON, XML, HTML, text and empty files.
	Copy-Item -Path (Join-Path $Source '*') -Destination $installDir -Recurse -Force
	foreach ($name in 'CnCNetYRLauncher.exe', 'gamemd.exe', 'gamemd-spawn.exe', 'Syringe.exe', 'game.exe', 'ra2md.mix') {
		Set-Content -LiteralPath (Join-Path $installDir $name) -Value "placeholder for $name, never run" -Encoding ascii
	}
	"install $installDir"
}

if ($Edit) {
	$definitions = Join-Path $copyDir 'Resources\ClientDefinitions.ini'
	if (-not (Test-Path -LiteralPath $definitions)) {
		Write-Error "fixture: no copy at $copyDir; create the copy on the page first"
		exit 1
	}
	# The copy's file is read-only, so the attribute goes first, as the updater's own write would need.
	(Get-Item -LiteralPath $definitions).IsReadOnly = $false
	Copy-Item -LiteralPath (Join-Path $installDir 'Resources\ClientDefinitions.ini') -Destination $definitions -Force
	"edits undone in $definitions"
}

if ($Lock) {
	New-Item -ItemType Directory -Force $lockedDir | Out-Null
	& icacls.exe $lockedDir /deny "${userName}:(WD)" | Out-Null
	"locked $lockedDir"
}

if ($Unlock) {
	if (Test-Path -LiteralPath $lockedDir) {
		& icacls.exe $lockedDir /remove:d $userName | Out-Null
		Remove-Item -LiteralPath $lockedDir -Recurse -Force -ErrorAction SilentlyContinue
	}
	"unlocked $lockedDir"
}

if ($Settings) {
	# The page reads its remembered choices from this section of the settings file.
	"[CnCNet]"
	"Install=$installDir"
	if ($Destination) { "Destination=$Destination" } else { "Destination=$copyDir" }
}
