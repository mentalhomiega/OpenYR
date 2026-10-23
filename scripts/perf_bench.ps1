# Runs the large-battle performance benchmark and summarises its Perf lines.
#
# The benchmark plays tests/autotest/bench-large-battle.txt (or -Script) on XMP08T2 at 1920x1080 with
# RenderFrameRate=240 and game speed 1, through scripts/run_autotest.ps1 in the workspace that holds this
# worktree (the folder above the worktree, with build-<Build>). Each run's debug log is copied to
# scratch-agent\perf as <Label>-run<N>.log. The summary averages every Perf second after the first
# -SkipSeconds of each run, so the placement at the start is left out.
#
# Summary columns: mean is the mean of the per-second averages, p95 is the 95th percentile of the
# per-second p95 values, and max is the largest per-second worst time. Frames, draws and presents are
# per second. Pass -Logs to summarise existing logs without running the game.
#
# Usage:
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/perf_bench.ps1 -Build agent14 -Runs 3
#   powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/perf_bench.ps1 -Build agent14 -Logs <file>...
param(
	[string]$Build = 'agent14',
	[string]$Script = '',
	[int]$Runs = 3,
	[int]$Seconds = 900,
	[int]$SkipSeconds = 5,
	[string]$Label = 'bench',
	[string]$OutDir = '',
	[string[]]$Logs = @()
)

$root = Split-Path $PSScriptRoot -Parent
$workspace = Split-Path $root -Parent
$runner = Join-Path $workspace 'scripts\run_autotest.ps1'
$debugDir = Join-Path $workspace "build-$Build\bin\Release\Debug"
if (-not $Script) { $Script = Join-Path $root 'tests\autotest\bench-large-battle.txt' }
if (-not $OutDir) { $OutDir = Join-Path $workspace 'scratch-agent\perf' }
New-Item -ItemType Directory -Force $OutDir | Out-Null

if ($Logs.Count -eq 0) {
	if (-not (Test-Path -LiteralPath $runner -PathType Leaf)) { Write-Error "perf_bench: no run_autotest.ps1 at $runner"; exit 1 }
	$scriptPath = (Resolve-Path -LiteralPath $Script).Path
	$Logs = @()
	for ($run = 1; $run -le $Runs; $run++) {
		"perf_bench: run $run of $Runs, $scriptPath"
		powershell.exe -NoProfile -ExecutionPolicy Bypass -File $runner -Build $Build -Script $scriptPath -Map XMP08T2.MAP -Width 1920 -Height 1080 -RenderFrameRate 240 -AIPlayers 3 -AICountries '5,5,5' -Seconds $Seconds -Filter 'BENCH|Exception' | Out-Host
		$log = Get-ChildItem (Join-Path $debugDir '*.LOG') | Sort-Object LastWriteTime | Select-Object -Last 1
		if (-not $log) { Write-Error "perf_bench: run $run wrote no debug log in $debugDir"; exit 1 }
		$copy = Join-Path $OutDir "$Label-run$run.log"
		Copy-Item $log.FullName $copy -Force
		$Logs += $copy
	}
}

# Phase names are prefixed by their line type, so the frame, draw and present phases can share names.
function Read-Records([string]$path) {
	$records = New-Object System.Collections.ArrayList
	$current = $null
	foreach ($line in [System.IO.File]::ReadLines($path)) {
		if ($line -match 'Perf: window (\d+) ms, (\d+) game frames, (\d+) draws, (\d+) presents') {
			$current = @{ Window = [double]$Matches[1]; Frames = [double]$Matches[2]; Draws = [double]$Matches[3]; Presents = [double]$Matches[4]; Phases = @{}; Counts = @{} }
			[void]$records.Add($current)
		} elseif ($current -and $line -match 'Perf (frame|draw|present) ms avg/p95/max: (.*)$') {
			$prefix = $Matches[1]
			foreach ($entry in ($Matches[2] -split ', ')) {
				if ($entry -match '^(.+) (\S+)/(\S+)/(\S+)$') {
					$current.Phases["$prefix.$($Matches[1])"] = [pscustomobject]@{ Avg = [double]$Matches[2]; P95 = [double]$Matches[3]; Max = [double]$Matches[4] }
				}
			}
		} elseif ($current -and $line -match 'Perf count: (.*)$') {
			foreach ($entry in ($Matches[1] -split ', ')) {
				if ($entry -match '^(.+) (\d+)$') { $current.Counts[$Matches[1]] = [double]$Matches[2] }
			}
		}
	}
	return ,$records
}

function Percentile([double[]]$values, [double]$fraction) {
	if ($values.Count -eq 0) { return 0.0 }
	$sorted = $values | Sort-Object
	$rank = [math]::Ceiling($fraction * $sorted.Count)
	return [double]$sorted[[math]::Max(0, $rank - 1)]
}

$Logs = @($Logs | ForEach-Object { $_ -split ',' } | ForEach-Object { $_.Trim() } | Where-Object { $_ })
$kept = New-Object System.Collections.ArrayList
foreach ($log in $Logs) {
	$records = Read-Records $log
	$skip = [math]::Min($SkipSeconds, $records.Count)
	for ($i = $skip; $i -lt $records.Count; $i++) { [void]$kept.Add($records[$i]) }
	"perf_bench: $log, $($records.Count) seconds, $skip skipped"
}
if ($kept.Count -eq 0) { Write-Error 'perf_bench: no Perf seconds to summarise'; exit 1 }

$out = New-Object System.Collections.ArrayList
[void]$out.Add("# Perf summary: $Label ($($kept.Count) seconds from $($Logs.Count) run(s))")
[void]$out.Add('')
$frames = @($kept | ForEach-Object { $_.Frames * 1000.0 / $_.Window })
$draws = @($kept | ForEach-Object { $_.Draws * 1000.0 / $_.Window })
$presents = @($kept | ForEach-Object { $_.Presents * 1000.0 / $_.Window })
[void]$out.Add("Game frames/s $([math]::Round(($frames | Measure-Object -Average).Average, 1)), draws/s $([math]::Round(($draws | Measure-Object -Average).Average, 1)), presents/s $([math]::Round(($presents | Measure-Object -Average).Average, 1))")
[void]$out.Add('')
[void]$out.Add('| Phase | Mean ms | p95 ms | Max ms |')
[void]$out.Add('|---|---:|---:|---:|')
$keys = @($kept[0].Phases.Keys)
foreach ($k in ($keys | Sort-Object { [array]::IndexOf(@('frame', 'draw', 'present'), ($_ -split '\.')[0]) }, { $_ })) {
	$present = @($kept | Where-Object { $_.Phases.ContainsKey($k) } | ForEach-Object { $_.Phases[$k] })
	$avgs = [double[]]@($present | ForEach-Object { $_.Avg })
	$p95s = [double[]]@($present | ForEach-Object { $_.P95 })
	$maxs = [double[]]@($present | ForEach-Object { $_.Max })
	$mean = ($avgs | Measure-Object -Average).Average
	$worst = ($maxs | Measure-Object -Maximum).Maximum
	[void]$out.Add(('| {0} | {1:N3} | {2:N3} | {3:N3} |' -f $k, $mean, (Percentile $p95s 0.95), $worst))
}
[void]$out.Add('')
[void]$out.Add('| Count (per second) | Mean |')
[void]$out.Add('|---|---:|')
foreach ($k in @($kept[0].Counts.Keys)) {
	$vals = [double[]]@($kept | Where-Object { $_.Counts.ContainsKey($k) } | ForEach-Object { $_.Counts[$k] })
	[void]$out.Add(('| {0} | {1:N0} |' -f $k, ($vals | Measure-Object -Average).Average))
}
$text = $out -join "`r`n"
$summary = Join-Path $OutDir "$Label-summary.md"
Set-Content -LiteralPath $summary -Value $text -Encoding utf8
$text
"perf_bench: wrote $summary"
