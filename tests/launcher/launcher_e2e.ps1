# End-to-end test of OpenYR.exe and its update copy, against a local feed and fake packages.
#
# The packages hold a dummy Game.exe, so nothing runs the game: the launcher and the update copy get
# /NOGAME. The setup gets /NOSHORTCUTS, so no Start Menu, desktop or Apps & Features entry is written.
# The launcher gets /QUIET, so no message box can wait for a click. Everything is written under a
# temporary folder that the script removes. The Yuri's Revenge folder is a fake holding two dummy archives.
#
# Safety: every setup call is checked before it starts. It must run silently, with /NOSHORTCUTS, and
# name its /DIR= under the test folder. Every launcher call must carry /QUIET and /NOSHORTCUTS, must not
# start the game (/NOGAME or /CHECKONLY), and may name paths only under the test folder. The script also
# compares the default install folder, the Start Menu and desktop shortcuts and the HKCU Uninstall key
# before and after, and fails if any of them changed.
#
# Usage: launcher_e2e.ps1 -Launcher <OpenYR.exe> -Setup <OpenYR-Setup.exe>
# Exit code 0 when every check passes, 1 otherwise.

param(
	[Parameter(Mandatory = $true)][string]$Launcher,
	[Parameter(Mandatory = $true)][string]$Setup,
	[switch]$KeepTemp
)

$ErrorActionPreference = 'Stop'
$Launcher = (Resolve-Path -LiteralPath $Launcher).Path
$Setup = (Resolve-Path -LiteralPath $Setup).Path
$defaultInstall = Join-Path $env:LOCALAPPDATA 'Programs\OpenYR'

# The stored-zip writer the packager uses: the setup reads stored entries only.
Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.IO;
public static class OpenYRStoredZip
{
    private static uint[] table;

    private static uint Crc(byte[] data)
    {
        if (table == null)
        {
            table = new uint[256];
            for (uint index = 0; index < 256; index++)
            {
                uint value = index;
                for (int bit = 0; bit < 8; bit++)
                {
                    value = (value & 1) != 0 ? 0xEDB88320u ^ (value >> 1) : value >> 1;
                }
                table[index] = value;
            }
        }

        uint crc = 0xFFFFFFFFu;
        foreach (byte item in data)
        {
            crc = table[(crc ^ item) & 0xFF] ^ (crc >> 8);
        }
        return crc ^ 0xFFFFFFFFu;
    }

    public static void Write(string root, string zipPath)
    {
        List<string> names = new List<string>();
        foreach (string file in Directory.GetFiles(root, "*", SearchOption.AllDirectories))
        {
            names.Add(file.Substring(root.Length).TrimStart('\\', '/').Replace('\\', '/'));
        }
        names.Sort(StringComparer.Ordinal);

        using (FileStream stream = File.Create(zipPath))
        using (BinaryWriter writer = new BinaryWriter(stream))
        {
            List<uint> offsets = new List<uint>();
            List<uint> sums = new List<uint>();
            List<int> sizes = new List<int>();
            List<ushort> dates = new List<ushort>();
            List<ushort> times = new List<ushort>();

            foreach (string name in names)
            {
                string path = Path.Combine(root, name.Replace('/', '\\'));
                byte[] data = File.ReadAllBytes(path);
                DateTime when = File.GetLastWriteTime(path);
                ushort time = (ushort)((when.Hour << 11) | (when.Minute << 5) | (when.Second / 2));
                ushort date = (ushort)(((when.Year - 1980) << 9) | (when.Month << 5) | when.Day);
                byte[] label = System.Text.Encoding.UTF8.GetBytes(name);
                uint crc = Crc(data);

                offsets.Add((uint)stream.Position);
                sums.Add(crc);
                sizes.Add(data.Length);
                dates.Add(date);
                times.Add(time);

                writer.Write(0x04034b50u);
                writer.Write((ushort)20);
                writer.Write((ushort)0);
                writer.Write((ushort)0);
                writer.Write(time);
                writer.Write(date);
                writer.Write(crc);
                writer.Write((uint)data.Length);
                writer.Write((uint)data.Length);
                writer.Write((ushort)label.Length);
                writer.Write((ushort)0);
                writer.Write(label);
                writer.Write(data);
            }

            uint directory = (uint)stream.Position;
            for (int index = 0; index < names.Count; index++)
            {
                byte[] label = System.Text.Encoding.UTF8.GetBytes(names[index]);
                writer.Write(0x02014b50u);
                writer.Write((ushort)20);
                writer.Write((ushort)20);
                writer.Write((ushort)0);
                writer.Write((ushort)0);
                writer.Write(times[index]);
                writer.Write(dates[index]);
                writer.Write(sums[index]);
                writer.Write((uint)sizes[index]);
                writer.Write((uint)sizes[index]);
                writer.Write((ushort)label.Length);
                writer.Write((ushort)0);
                writer.Write((ushort)0);
                writer.Write((ushort)0);
                writer.Write((ushort)0);
                writer.Write((uint)0);
                writer.Write(offsets[index]);
                writer.Write(label);
            }
            uint directorySize = (uint)stream.Position - directory;

            writer.Write(0x06054b50u);
            writer.Write((ushort)0);
            writer.Write((ushort)0);
            writer.Write((ushort)names.Count);
            writer.Write((ushort)names.Count);
            writer.Write(directorySize);
            writer.Write(directory);
            writer.Write((ushort)0);
        }
    }
}
'@

$script:Failures = 0
function Check([bool]$ok, [string]$what) {
	$mark = if ($ok) { 'ok' } else { 'FAILED' }
	Write-Output ('{0,-72} {1}' -f $what, $mark)
	if (-not $ok) { $script:Failures++ }
}

# A path must lie under the test folder; nothing outside it is touched.
function Assert-Under([string]$path, [string]$folder, [string]$what) {
	$full = [IO.Path]::GetFullPath($path).TrimEnd('\') + '\'
	$base = [IO.Path]::GetFullPath($folder).TrimEnd('\') + '\'
	if (-not $full.StartsWith($base, [StringComparison]::OrdinalIgnoreCase)) {
		throw "SAFETY: $what '$path' is outside the test folder '$folder'."
	}
}

# Every path named in an argument string must lie under the test folder. URLs are not paths.
function Assert-Paths([string]$arguments) {
	$pattern = '/(?:DIR|FOLDER|YR|LOG|FEED|SETUPURL|SUMURL)=(?:"([^"]*)"|(\S+))'
	foreach ($match in [regex]::Matches($arguments, $pattern, 'IgnoreCase')) {
		$value = if ($match.Groups[1].Success) { $match.Groups[1].Value } else { $match.Groups[2].Value }
		if ($value -match '^https?://') { continue }
		Assert-Under $value $root 'a path in the arguments'
	}
}

function Assert-Setup-Args([string]$arguments) {
	if ($arguments -notmatch '(^|\s)/S(\s|$)') { throw 'SAFETY: the setup must run silently (/S).' }
	if ($arguments -notmatch '(^|\s)/NOSHORTCUTS(\s|$)') { throw 'SAFETY: the setup must run with /NOSHORTCUTS.' }
	if ($arguments -notmatch '/DIR=') { throw 'SAFETY: the setup must name its folder with /DIR=.' }
	Assert-Paths $arguments
}

function Assert-Launcher-Args([string]$arguments) {
	if ($arguments -notmatch '(^|\s)/QUIET(\s|$)') { throw 'SAFETY: the launcher must run with /QUIET.' }
	if ($arguments -notmatch '(^|\s)/NOSHORTCUTS(\s|$)') { throw 'SAFETY: the launcher must run with /NOSHORTCUTS.' }
	if ($arguments -notmatch '(^|\s)/NOGAME(\s|$)' -and $arguments -notmatch '(^|\s)/CHECKONLY(\s|$)') {
		throw 'SAFETY: the launcher must not start the game: pass /NOGAME or /CHECKONLY.'
	}
	Assert-Paths $arguments
}

# Checks the guards refuse the dangerous calls and accept a safe one. Nothing is started here.
function Test-Guards {
	$refused = 0
	$attempts = @(
		{ Assert-Setup-Args ('/S /NOSHORTCUTS /DIR="' + $defaultInstall + '"') },
		{ Assert-Setup-Args ('/S /DIR="' + $root + '\x"') },
		{ Assert-Launcher-Args '/CHECKONLY /QUIET' },
		{ Assert-Launcher-Args ('/NOGAME /QUIET /NOSHORTCUTS /NOSHORTCUTS /LOG="' + $defaultInstall + '\user\x.log"') }
	)
	foreach ($attempt in $attempts) {
		try { & $attempt; } catch { $refused++ }
	}
	Check ($refused -eq $attempts.Count) 'the guards refuse each unsafe call before anything starts'

	$accepted = $true
	try { Assert-Setup-Args ('/S /NOSHORTCUTS /DIR="' + $root + '\x"'); Assert-Launcher-Args ('/NOGAME /QUIET /NOSHORTCUTS /NOSHORTCUTS /FEED="' + $root + '\f.json"') } catch { $accepted = $false }
	Check $accepted 'and they accept a call that stays in the test folder'
}

# The parts of the real machine a test must not change: the default install, its shortcuts and the HKCU key.
function Get-Real-State {
	$lines = New-Object System.Collections.Generic.List[string]
	if (Test-Path -LiteralPath $defaultInstall) {
		foreach ($item in Get-ChildItem -LiteralPath $defaultInstall -Recurse -Force) {
			$lines.Add('F|' + $item.FullName + '|' + $item.Length + '|' + $item.LastWriteTimeUtc.Ticks)
		}
	}
	$startMenu = Join-Path $env:APPDATA 'Microsoft\Windows\Start Menu\Programs\OpenYR'
	$desktop = Join-Path ([Environment]::GetFolderPath('Desktop')) 'OpenYR.lnk'
	foreach ($item in @(Get-ChildItem -LiteralPath $startMenu -Force -ErrorAction SilentlyContinue) + @(Get-Item -LiteralPath $desktop -Force -ErrorAction SilentlyContinue)) {
		if ($item) { $lines.Add('S|' + $item.FullName + '|' + $item.LastWriteTimeUtc.Ticks) }
	}
	$key = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenYR'
	if (Test-Path -LiteralPath $key) {
		foreach ($property in (Get-ItemProperty -LiteralPath $key).PSObject.Properties) {
			if ($property.Name -notlike 'PS*') { $lines.Add('R|' + $property.Name + '|' + $property.Value) }
		}
	} else {
		$lines.Add('R|absent')
	}
	return ($lines -join "`n")
}

# Runs an executable with one argument string and returns its exit code. The call is checked first.
function Run-Exe([string]$kind, [string]$exe, [string]$arguments) {
	if ($kind -eq 'setup') { Assert-Setup-Args $arguments } else { Assert-Launcher-Args $arguments }
	Assert-Under $exe $root 'the executable'
	$process = Start-Process -FilePath $exe -ArgumentList $arguments -WindowStyle Hidden -Wait -PassThru
	return $process.ExitCode
}

function Read-Text([string]$path) {
	if (-not (Test-Path -LiteralPath $path)) { return '' }
	return [IO.File]::ReadAllText($path)
}

# A setup for one version: the setup executable with a stored package of the given files appended.
function New-Setup([string]$version, [string]$engine, [string]$output) {
	$stage = Join-Path $root ('stage-' + $version)
	Remove-Item -LiteralPath $stage -Recurse -Force -ErrorAction SilentlyContinue
	New-Item -ItemType Directory -Force (Join-Path $stage 'user'), (Join-Path $stage 'ui') | Out-Null
	[IO.File]::WriteAllText((Join-Path $stage 'Game.exe'), $engine, [Text.Encoding]::ASCII)
	Copy-Item -LiteralPath $Launcher -Destination (Join-Path $stage 'OpenYR.exe')
	[IO.File]::WriteAllText((Join-Path $stage 'Language.dll'), 'language', [Text.Encoding]::ASCII)
	[IO.File]::WriteAllText((Join-Path $stage 'LICENSE.md'), 'licence', [Text.Encoding]::ASCII)
	[IO.File]::WriteAllText((Join-Path $stage 'OPENYR-BUILD.txt'), "Build=$version`r`nCommit=e2e`r`n", [Text.Encoding]::ASCII)
	[IO.File]::WriteAllText((Join-Path $stage 'user\RA2MD.INI'), "[Video]`r`nRenderFrameRate=240`r`n", [Text.Encoding]::ASCII)

	$zip = Join-Path $root ('payload-' + $version + '.zip')
	[OpenYRStoredZip]::Write($stage, $zip)

	$stub = [IO.File]::ReadAllBytes($Setup)
	$payload = [IO.File]::ReadAllBytes($zip)
	$trailer = New-Object System.Collections.Generic.List[byte]
	$trailer.AddRange([BitConverter]::GetBytes([uint64]$stub.Length))
	$trailer.AddRange([BitConverter]::GetBytes([uint64]$payload.Length))
	$trailer.AddRange([Text.Encoding]::ASCII.GetBytes('OPYRPAY1'))
	$file = [IO.File]::Create($output)
	try {
		$file.Write($stub, 0, $stub.Length)
		$file.Write($payload, 0, $payload.Length)
		$bytes = $trailer.ToArray()
		$file.Write($bytes, 0, $bytes.Length)
	} finally {
		$file.Dispose()
	}
}

# A release feed as the GitHub API names it, with the two assets as local paths.
function New-Feed([string]$tag, [string]$setupPath, [string]$sumPath, [string]$output) {
	$feed = [ordered]@{
		tag_name = $tag
		draft = $false
		prerelease = $false
		assets = @(
			[ordered]@{ name = "OpenYR-$tag-Setup.exe"; browser_download_url = $setupPath },
			[ordered]@{ name = "OpenYR-$tag-Setup.exe.sha256"; browser_download_url = $sumPath }
		)
	}
	[IO.File]::WriteAllText($output, ($feed | ConvertTo-Json -Depth 5), [Text.Encoding]::UTF8)
}

$root = Join-Path ([IO.Path]::GetTempPath()) ('openyr-launcher-e2e-' + $PID)
Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $root | Out-Null

$realBefore = $null
try {
	$realBefore = Get-Real-State
	Test-Guards
	$yuri = Join-Path $root 'Yuri'
	$install = Join-Path $root 'Programs\OpenYR'
	$logs = Join-Path $root 'logs'
	New-Item -ItemType Directory -Force $yuri, $logs | Out-Null
	[IO.File]::WriteAllText((Join-Path $yuri 'ra2md.mix'), 'dummy', [Text.Encoding]::ASCII)
	[IO.File]::WriteAllText((Join-Path $yuri 'ra2.mix'), 'dummy', [Text.Encoding]::ASCII)

	$setup22 = Join-Path $root 'OpenYR-beta22-Setup.exe'
	$setup23 = Join-Path $root 'OpenYR-beta23-Setup.exe'
	New-Setup 'beta22' 'engine beta22' $setup22
	New-Setup 'beta23' 'engine beta23' $setup23

	$sum23 = Join-Path $root 'OpenYR-beta23-Setup.exe.sha256'
	$hash23 = (Get-FileHash -Algorithm SHA256 -LiteralPath $setup23).Hash.ToLower()
	[IO.File]::WriteAllText($sum23, "$hash23  OpenYR-beta23-Setup.exe`n", [Text.Encoding]::ASCII)
	$badSum = Join-Path $root 'bad.sha256'
	[IO.File]::WriteAllText($badSum, ('0' * 64) + "`n", [Text.Encoding]::ASCII)

	$feed22 = Join-Path $root 'feed-beta22.json'
	$feed23 = Join-Path $root 'feed-beta23.json'
	New-Feed 'beta22' $setup22 $setup22 $feed22
	New-Feed 'beta23' $setup23 $sum23 $feed23

	# Install beta22 silently, as a player's first install would.
	$code = Run-Exe 'setup' $setup22 ('/S /DIR="' + $install + '" /YR="' + $yuri + '" /NOSHORTCUTS /LOG="' + (Join-Path $logs 'install22.log') + '"')
	Check ($code -eq 0) 'the setup installs beta22 silently with exit code 0'
	$record = Read-Text (Join-Path $install 'openyr-install.ini')
	Check ($record.Contains('Build=beta22')) 'the install record names beta22'
	Check (Test-Path -LiteralPath (Join-Path $install 'OpenYR.exe')) 'the launcher is installed beside Game.exe'

	# The player's own settings: a feed under [Update], and files under user that an update must keep.
	Add-Content -LiteralPath (Join-Path $install 'OPENTS.INI') -Value "[Update]`r`nFeedURL=$feed23`r`n" -Encoding ascii
	New-Item -ItemType Directory -Force (Join-Path $install 'user\Saved Games') | Out-Null
	[IO.File]::WriteAllText((Join-Path $install 'user\Saved Games\mission.sav'), 'a save', [Text.Encoding]::ASCII)

	$log = Join-Path $logs 'check-feed.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/CHECKONLY /QUIET /NOSHORTCUTS /LOG="' + $log + '"')
	$text = Read-Text $log
	Check ($code -eq 0) 'a check-only run from the install exits with 0'
	Check ($text.Contains('would ask to update from Beta 22 to Beta 23')) 'the check reads the feed from [Update] FeedURL and offers beta23'
	Check ($text.Contains('Nothing was downloaded or changed')) 'the check-only run says it changed nothing'
	Check ($text.Contains('the update copy would start as:') -and $text.Contains('/UPDATE=beta23') -and $text.Contains('/WAITFOR=')) 'the check-only run prints the update copy command it would start'
	Check ((Read-Text (Join-Path $install 'Game.exe')) -eq 'engine beta22') 'and Game.exe is still beta22'

	$log = Join-Path $logs 'check-same.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/CHECKONLY /QUIET /NOSHORTCUTS /FEED="' + $feed22 + '" /LOG="' + $log + '"')
	Check ((Read-Text $log).Contains('not newer than the Beta 22 installed')) 'a feed naming the installed beta offers nothing'

	$log = Join-Path $logs 'check-http.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/CHECKONLY /QUIET /NOSHORTCUTS /FEED=http://127.0.0.1:9/feed /LOG="' + $log + '"')
	Check ((Read-Text $log).Contains('must start with https://')) 'a plain http feed is refused and the game would start as before'

	$log = Join-Path $logs 'check-missing.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/CHECKONLY /QUIET /NOSHORTCUTS /FEED="' + (Join-Path $root 'missing.json') + '" /LOG="' + $log + '"')
	Check ((Read-Text $log).Contains('Update check skipped: Could not read')) 'a missing feed file is logged and the run goes on'

	$log = Join-Path $logs 'quiet-offer.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/NOGAME /QUIET /NOSHORTCUTS /FEED="' + $feed23 + '" /LOG="' + $log + '"')
	Check ($code -eq 0 -and (Read-Text $log).Contains('but no question is asked')) 'a quiet run that finds beta23 asks nothing and starts the version it has'

	$log = Join-Path $logs 'normal-run.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/NOGAME /QUIET /NOSHORTCUTS /FEED="' + $feed22 + '" /LOG="' + $log + '"')
	Check ($code -eq 0 -and (Read-Text $log).Contains('Not starting Game.exe (/NOGAME).')) 'a normal run with nothing newer starts the game path and exits with 0'

	# The update copy, as the launcher starts it. A bad checksum stops the update before the setup runs.
	$helper = Join-Path $root 'helper-badsum'
	New-Item -ItemType Directory -Force $helper | Out-Null
	Copy-Item -LiteralPath $Launcher -Destination (Join-Path $helper 'OpenYR-Update.exe')
	$log = Join-Path $logs 'update-badsum.log'
	$arguments = '/UPDATE=beta23 /SETUPURL="' + $setup23 + '" /SUMURL="' + $badSum + '" /FOLDER="' + $install + '" /YR="' + $yuri + '" /LOG="' + $log + '" /NOGAME /QUIET /NOSHORTCUTS -- -SPAWN'
	$code = Run-Exe 'launcher' (Join-Path $helper 'OpenYR-Update.exe') $arguments
	Check ($code -eq 1) 'an update copy with a bad checksum exits with 1'
	Check ((Read-Text $log).Contains('does not match its checksum')) 'and says the download did not match'
	Check ((Read-Text (Join-Path $install 'openyr-install.ini')).Contains('Build=beta22')) 'the install is still beta22'

	# A setup that refuses the Yuri's Revenge folder leaves the install alone.
	$helper = Join-Path $root 'helper-noyuri'
	New-Item -ItemType Directory -Force $helper | Out-Null
	Copy-Item -LiteralPath $Launcher -Destination (Join-Path $helper 'OpenYR-Update.exe')
	$log = Join-Path $logs 'update-noyuri.log'
	$arguments = '/UPDATE=beta23 /SETUPURL="' + $setup23 + '" /SUMURL="' + $sum23 + '" /FOLDER="' + $install + '" /YR="' + (Join-Path $root 'nowhere') + '" /LOG="' + $log + '" /NOGAME /QUIET /NOSHORTCUTS -- -SPAWN'
	$code = Run-Exe 'launcher' (Join-Path $helper 'OpenYR-Update.exe') $arguments
	Check ($code -eq 1 -and (Read-Text $log).Contains('the setup exit code is 4')) 'a setup that finds no Yuri''s Revenge exits with 4, and the update copy reports it'
	Check ((Read-Text (Join-Path $install 'openyr-install.ini')).Contains('Build=beta22')) 'and the install is still beta22'

	# The update itself: download, check, run the setup over the install, then start the game (not here).
	$helper = Join-Path $root 'helper-update'
	New-Item -ItemType Directory -Force $helper | Out-Null
	Copy-Item -LiteralPath $Launcher -Destination (Join-Path $helper 'OpenYR-Update.exe')
	$log = Join-Path $logs 'update.log'
	# /WAITFOR names a process the copy waits for before the setup runs; a short ping stands in for the launcher.
	$waiter = Start-Process -FilePath 'cmd.exe' -ArgumentList '/c ping -n 3 127.0.0.1 >nul' -WindowStyle Hidden -PassThru
	$arguments = '/UPDATE=beta23 /SETUPURL="' + $setup23 + '" /SUMURL="' + $sum23 + '" /FOLDER="' + $install + '" /YR="' + $yuri + '" /LOG="' + $log + '" /WAITFOR=' + $waiter.Id + ' /NOGAME /QUIET /NOSHORTCUTS -- -SPAWN'
	$code = Run-Exe 'launcher' (Join-Path $helper 'OpenYR-Update.exe') $arguments
	$text = Read-Text $log
	Check ($code -eq 0) 'the update copy exits with 0 when the update installs'
	Check ($text.Contains('has closed.')) 'the update copy waits for the process named by /WAITFOR before it runs the setup'
	Check ($text.Contains('Updated to Beta 23.')) 'the log says the install is now beta23'
	Check ($text.Contains('Not starting Game.exe (/NOGAME).')) 'and it would start Game.exe, which the test skips'
	Check ((Read-Text (Join-Path $install 'Game.exe')) -eq 'engine beta23') 'Game.exe is now the beta23 package'
	Check ((Read-Text (Join-Path $install 'openyr-install.ini')).Contains('Build=beta23')) 'the install record names beta23'
	$config = Read-Text (Join-Path $install 'OPENTS.INI')
	Check ($config.Contains('[Update]') -and $config.Contains("FeedURL=$feed23")) 'the update keeps the [Update] settings in OPENTS.INI'
	Check ($config.Contains('DataDirectory=' + $yuri)) 'and points DataDirectory at the same Yuri''s Revenge folder'
	Check ((Read-Text (Join-Path $install 'user\Saved Games\mission.sav')) -eq 'a save') 'the update keeps the player''s saves under user'
	$installed = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $install 'OpenYR.exe')).Hash
	$built = (Get-FileHash -Algorithm SHA256 -LiteralPath $Launcher).Hash
	Check ($installed -eq $built) 'the launcher in the install is the one from the package'

	$log = Join-Path $logs 'check-after.log'
	$code = Run-Exe 'launcher' (Join-Path $install 'OpenYR.exe') ('/CHECKONLY /QUIET /NOSHORTCUTS /LOG="' + $log + '"')
	Check ((Read-Text $log).Contains('not newer than the Beta 23 installed')) 'after the update the same feed offers nothing'
}
finally {
	$realAfter = Get-Real-State
	$realChanged = $null -eq $realBefore -or $realBefore -ne $realAfter
	Check (-not $realChanged) 'the default install, its shortcuts and the HKCU Uninstall key are unchanged'
	if ($realChanged -and $null -ne $realBefore) {
		# Printed so a change made by someone else during the run is visible; the test writes nothing there.
		$differences = Compare-Object -ReferenceObject ($realBefore -split "`n") -DifferenceObject ($realAfter -split "`n") | Select-Object -First 12
		foreach ($difference in $differences) { Write-Output ('  changed: ' + $difference.SideIndicator + ' ' + $difference.InputObject) }
	}
	if (-not $KeepTemp) {
		Assert-Under $root ([IO.Path]::GetTempPath()) 'the test folder'
		Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
	}
}

if ($script:Failures -ne 0) {
	Write-Output "$($script:Failures) check(s) failed."
	exit 1
}
Write-Output 'All launcher end-to-end checks passed.'
exit 0
