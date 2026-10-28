# Runs two copies of one build's Game.exe on this machine as a two-player network match over
# loopback, each with -SYNCLOG, then compares the frame check values the two copies logged.
# It reports the ticks each side played, whether either side reported an out-of-sync game, and the
# first frame whose check values differ. When the game reports a desync it also lists the first
# lines where the two out-of-sync reports differ, from their object sections on.
#
# -GameExe is the Game.exe to run and -DataDir the Yuri's Revenge folder that holds the game data.
# -WorkDir holds each copy's user folder and the relay log; it defaults to a netsync folder beside
# Game.exe. -Map is a skirmish map in the game data.
# -Seconds is how long both copies run before they are stopped. -AI seats that many computer players,
# the same way on both copies. -AICountries gives their countries in order, from the third seat on.
# -ScriptA and -ScriptB are optional autotest scripts for each copy. The runner adds a log line at
# frame 0 that names the copy, so it can find each copy's debug log. -KeepDir copies both logs and
# any out-of-sync reports there, because the next run clears the build's Debug folder.
# -GameMode writes the multiplayer mode's MPModesMD.ini index into both launch files; 0 leaves the mode alone.
# -GameSpeed writes the game speed (0 fastest, 6 slowest) into both launch files. -HostLast seats the host
# copy in the higher seat, so the other copy is the lower one.
#
# Each copy binds 127.0.0.1 (the BindAddress key), so nothing listens on the network and no firewall
# rule is needed. Both copies run hidden and muted, with unattended settings: no movies, and no
# question about sending problem reports. The copies share the Debug folder beside Game.exe, so
# runs of one Game.exe take turns, and at most four games run at once on the machine. Only the
# PIDs this runner started are stopped.
#
# -SpawnStyle CnCNet writes each launch file the way the CnCNet client does, as far as its
# WriteSpawnIni code is known (see New-CnCNetSpawnIni for the placeholders). The copies then get
# only the client's -SPAWN plus the harness switches, and the sync log comes from RA2MD.INI.
#
# -Tunnel routes both copies through a local tunnel. Each copy's [Tunnel] section names its own
# bridge port, and the copies name each other by tunnel number. scripts/cncnet_tunnel_relay.py runs
# with python -I on RelayPort, as bridges and a relay, and -LatencyMs, -JitterMs and -LossPercent
# act on its relay hop. Each datagram crosses the hop once in each direction, so -LatencyMs 50 gives
# about 100 ms round trip. In tunnel mode the game's own socket binds an ephemeral port on every
# address, because the engine ignores BindAddress there; the runner's listing shows that address.
#
# Exit codes: 0 both sides in sync over the common ticks; 3 a desync was reported or the check
# values differ; 4 the match produced no common ticks; 1 the runner itself could not start.
param(
    [int]$Seconds = 120,
    [string]$Map = 'XMP29U2.MAP',
    [ValidateRange(0, 6)][int]$AI = 2,
    [string]$AICountries = '',
    # The Game.exe to run, and the Yuri's Revenge folder with the game data. Both are required.
    [string]$GameExe = '',
    [string]$DataDir = '',
    [string]$WorkDir = '',
    [string]$ScriptA = '',
    [string]$ScriptB = '',
    [int]$Seed = 1,
    # The game speed both launch files write: 0 is fastest (60 FPS) and 6 is slowest (10 FPS).
    [ValidateRange(0, 6)][int]$GameSpeed = 1,
    # Seats the host copy (side A) in the higher seat (side 8, colour 1), and the other copy in seat 0.
    [switch]$HostLast,
    # Writes GameMode into both launch files: the MPModesMD.ini index of the mode the match plays. 0
    # writes no key, so the match plays Battle. An index the file does not list runs as Battle, which
    # is how a bogus index is tested. The listed indices are 1 Battle, 2 FreeForAll, 3 Cooperative,
    # 4 Unholy Alliance, 5 Megawealth, 6 Duel, 7 Meat-Grinder, 8 Naval War and 9 Team Game.
    [ValidateRange(0, 999)][int]$GameMode = 0,
    [ValidateRange(1024, 65535)][int]$PortA = 24601,
    [ValidateRange(1024, 65535)][int]$PortB = 24602,
    [int]$Width = 640,
    [int]$Height = 480,
    [string]$KeepDir = '',
    # Corrupts side B's check value at this frame, as -DESYNCTEST= does, so the detection can be
    # checked against a known mismatch. 0 leaves the copies alone; use a frame past about 300.
    [int]$ForceDesyncB = 0,
    # Adds a third human seat, side C, that joins sides A and B; A and B list it as [Other2]. -PortC is its port.
    # -ForceDesyncC corrupts side C's check value at that frame, and -ScriptC is its autotest script. Runner style only.
    [switch]$ThirdSeat,
    [ValidateRange(1024, 65535)][int]$PortC = 24603,
    [int]$ForceDesyncC = 0,
    [string]$ScriptC = '',
    # Free player colours as r,g,b (the spawn.ini ColorRGB entry) for side A and side B. Empty
    # leaves the stock Color index alone.
    [string]$ColorRGBA = '',
    [string]$ColorRGBB = '',
    # Runner writes the test's own launch files. CnCNet writes them the way the CnCNet client does
    # (its key names and the values it writes, as far as they are known), and sets SyncLog in
    # RA2MD.INI instead of passing -SYNCLOG, because the client passes only -SPAWN.
    [ValidateSet('Runner', 'CnCNet')][string]$SpawnStyle = 'Runner',
    # The in-game interface both copies run, as [Options] InterfaceStyle. Modern is the game's default from Beta 24.
    [ValidateSet('Classic', 'Modern')][string]$InterfaceStyle = 'Classic',
    # Routes both copies through a local tunnel: one bridge port per copy, and the relay that
    # scripts/cncnet_tunnel_relay.py runs on RelayPort. The copies then name each other by tunnel
    # number, as the client's V3 tunnel does. -LatencyMs, -JitterMs and -LossPercent apply to the relay hop.
    [switch]$Tunnel,
    [ValidateRange(1024, 65535)][int]$RelayPort = 24650,
    [ValidateRange(1024, 65535)][int]$BridgePortA = 24651,
    [ValidateRange(1024, 65535)][int]$BridgePortB = 24652,
    [ValidateRange(-65535, 65535)][int]$TunnelIdA = 1001,
    [ValidateRange(-65535, 65535)][int]$TunnelIdB = 1002,
    [ValidateRange(0, 10000)][double]$LatencyMs = 0,
    [ValidateRange(0, 10000)][double]$JitterMs = 0,
    [ValidateRange(0, 100)][double]$LossPercent = 0,
    [int]$RelaySeed = 1,
    # Largest envelope the relay accepts. The CnCNet server source reads into 1024 bytes, and a global
    # message is 1076 bytes here, so a run at 1024 loses those messages. 2048 is the test's own setting.
    [ValidateRange(1024, 65535)][int]$RelayBuffer = 1024,
    # The host's network timing choice, written into both launch files as FrameSendRate, MaxAhead and
    # Protocol. 0 writes no key (Runner style) or the CnCNet client's own default (CnCNet style), which the
    # game reads as no choice. FrameSendRate is 1 to 10 and MaxAhead 4 to 250 (rounded up to whole send periods).
    [ValidateRange(0, 99)][int]$FrameSendRate = 0,
    [ValidateRange(0, 999)][int]$MaxAhead = 0,
    [ValidateRange(-1, 2)][int]$Protocol = -1
)

$root = Split-Path $PSScriptRoot -Parent
if (-not $GameExe -or -not (Test-Path -LiteralPath $GameExe -PathType Leaf)) {
    "netsync: -GameExe must name an existing Game.exe"
    exit 1
}
if (-not $DataDir -or -not (Test-Path -LiteralPath $DataDir -PathType Container)) {
    "netsync: -DataDir must name the Yuri's Revenge folder that holds the game data"
    exit 1
}
$bin = Split-Path (Resolve-Path -LiteralPath $GameExe).Path -Parent
if (-not $WorkDir) { $WorkDir = Join-Path $bin 'netsync' }
New-Item -ItemType Directory -Force $WorkDir | Out-Null
$userA = Join-Path $WorkDir 'side-a-user'
$userB = Join-Path $WorkDir 'side-b-user'
$userC = Join-Path $WorkDir 'side-c-user'
$logs = Join-Path $bin 'Debug'
$exe = (Resolve-Path -LiteralPath $GameExe).Path

if ($PortA -eq $PortB) {
    "netsync: -PortA and -PortB must differ"
    exit 1
}
if ($ThirdSeat) {
    if ($Tunnel -or $SpawnStyle -ne 'Runner') {
        "netsync: -ThirdSeat works with the Runner spawn style and without -Tunnel"
        exit 1
    }
    if ((@($PortA, $PortB, $PortC) | Sort-Object -Unique).Count -ne 3) {
        "netsync: -PortA, -PortB and -PortC must differ"
        exit 1
    }
}
$relayScript = Join-Path $root 'scripts\cncnet_tunnel_relay.py'
if ($Tunnel) {
    if ($TunnelIdA -eq 0 -or $TunnelIdB -eq 0 -or (($TunnelIdA -band 0xFFFF) -eq ($TunnelIdB -band 0xFFFF))) {
        "netsync: -TunnelIdA and -TunnelIdB must be nonzero and differ in their low 16 bits"
        exit 1
    }
    if ((@($RelayPort, $BridgePortA, $BridgePortB) | Sort-Object -Unique).Count -ne 3) {
        "netsync: -RelayPort, -BridgePortA and -BridgePortB must differ"
        exit 1
    }
    if (-not (Test-Path -LiteralPath $relayScript -PathType Leaf)) {
        "netsync: relay script $relayScript not found"
        exit 1
    }
}
if ($AICountries) {
    $countryList = @($AICountries.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ })
    if ($countryList.Count -lt $AI) {
        "netsync: -AICountries names $($countryList.Count) countries for $AI computer players"
        exit 1
    }
}
foreach ($script in @($ScriptA, $ScriptB, $ScriptC)) {
    if ($script -and -not (Test-Path -LiteralPath $script -PathType Leaf)) {
        "netsync: script '$script' not found"
        exit 1
    }
}

# One lock per Game.exe folder, so runs of one build take turns. A runner that was killed leaves
# its lock abandoned, which the next one takes over. A stuck runner makes the next one give up.
$lockMinutes = 30
$lockId = ([BitConverter]::ToString([Security.Cryptography.SHA1]::Create().ComputeHash([Text.Encoding]::UTF8.GetBytes($bin.ToLowerInvariant()))) -replace '-', '').Substring(0, 16)
$buildLock = New-Object System.Threading.Mutex($false, "Global\OpenYRNetsync-$lockId")
$locked = $false
try { $locked = $buildLock.WaitOne([TimeSpan]::FromMinutes($lockMinutes)) } catch [System.Threading.AbandonedMutexException] { $locked = $true }
if (-not $locked) {
    "netsync gave up: another runner held $bin for $lockMinutes minutes"
    $buildLock.Dispose()
    exit 1
}

# Game slots shared by every runner on this machine, so at most four games run at once.
$gameSlots = 0..3 | ForEach-Object { New-Object System.Threading.Mutex($false, "Global\OpenYRAutotestGame$_") }
$held = @()
$exitCode = 1
try {

$slotDeadline = (Get-Date).AddMinutes($lockMinutes)
$slotsNeeded = if ($ThirdSeat) { 3 } else { 2 }
while ($held.Count -lt $slotsNeeded) {
    foreach ($m in $gameSlots) {
        if ($held -contains $m) { continue }
        $got = $false
        try { $got = $m.WaitOne(0) } catch [System.Threading.AbandonedMutexException] { $got = $true }
        if ($got) { $held += $m; if ($held.Count -eq $slotsNeeded) { break } }
    }
    if ($held.Count -lt $slotsNeeded) {
        if ((Get-Date) -gt $slotDeadline) { "netsync gave up: fewer than $slotsNeeded game slots free for $lockMinutes minutes"; exit 1 }
        Start-Sleep -Seconds 2
    }
}

$userList = if ($ThirdSeat) { @($userA, $userB, $userC) } else { @($userA, $userB) }
foreach ($user in $userList) { New-Item -ItemType Directory -Force $user | Out-Null }

# The GameMode line in [Settings]; empty when -GameMode is 0. The CnCNet writer below has no known
# key for the mode, so the line it gets is the runner's own.
$modeLine = if ($GameMode -gt 0) { "GameMode=$GameMode`r`n" } else { '' }
# The timing lines of the launch file. The Runner style writes a key only when it was asked for. The CnCNet
# style always writes the client's defaults (send period 7, protocol 2, no look-ahead), as the client does.
# The Runner style adds HostTiming=yes, which makes the game take the values literally.
$timingLines = ''
if ($FrameSendRate -gt 0 -or $MaxAhead -gt 0 -or $Protocol -ge 0) { $timingLines += "HostTiming=yes`r`n" }
if ($FrameSendRate -gt 0) { $timingLines += "FrameSendRate=$FrameSendRate`r`n" }
if ($MaxAhead -gt 0) { $timingLines += "MaxAhead=$MaxAhead`r`n" }
if ($Protocol -ge 0) { $timingLines += "Protocol=$Protocol`r`n" }
$cncnetFrameSendRate = if ($FrameSendRate -gt 0) { $FrameSendRate } else { 7 }
$cncnetMaxAheadLine = if ($MaxAhead -gt 0) { "MaxAhead=$MaxAhead`r`n" } else { '' }
$cncnetProtocol = if ($Protocol -ge 0) { $Protocol } else { 2 }

$aiBlock = ''
if ($AICountries -and $AI -gt 0) {
    $countryList = @($AICountries.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ })
    $aiLines = for ($n = 0; $n -lt $AI; $n++) { "Multi$($n + 3)=$($countryList[$n])" }
    $aiBlock = "`r`n[HouseCountries]`r`n" + ($aiLines -join "`r`n") + "`r`n"
}

# Both copies seat the same two people and the same computer players, so their houses line up.
# With -Tunnel, Port names the copy's tunnel number, and a [Tunnel] section points it at its bridge.
function New-SpawnIni([string]$Name, [int]$Side, [int]$Color, [int]$Port, [string]$HostFlag,
                      [string]$PeerName, [int]$PeerSide, [int]$PeerColor, [int]$PeerPort, [int]$BridgePort = 0) {
$tunnelBlock = if ($BridgePort -gt 0) { "`r`n[Tunnel]`r`nIp=127.0.0.1`r`nPort=$BridgePort`r`n" } else { '' }
@"
[Settings]
Scenario=$Map
Name=$Name
Side=$Side
Color=$Color
Port=$Port
Host=$HostFlag
BindAddress=127.0.0.1
AIPlayers=$AI
$modeLine
$timingLines
Credits=10000
UnitCount=0
GameSpeed=$GameSpeed
Seed=$Seed
ShortGame=no

[Other1]
Name=$PeerName
Side=$PeerSide
Color=$PeerColor
Ip=127.0.0.1
Port=$PeerPort
$aiBlock
$tunnelBlock
"@
}

# Writes the launch file the CnCNet client writes for a two-person game with computer players, from
# the keys and values in its GameLobbyBase.WriteSpawnIni. Three values the client takes from the map
# or game options are placeholders: UIGameMode, BroadcastedGameOptionValues and GamePackageVersion.
# GameID is a placeholder too, and MapSHA1 is empty, because the map lives in an archive. MapID is
# left out, since the client writes it only for official maps. The client writes the map as
# spawnmap.ini; here Scenario names the map in the game data. The client also writes SpawnLocations,
# IsSpectator sections and alliances only when they apply, and this writer leaves them out.
function New-CnCNetSpawnIni([string]$Name, [int]$Side, [int]$Color, [int]$Port, [string]$HostFlag,
                            [string]$PeerName, [int]$PeerSide, [int]$PeerColor, [int]$PeerPort,
                            [string]$PeerIp, [int]$BridgePort = 0) {
    $mapBase = [System.IO.Path]::GetFileNameWithoutExtension($Map)
    $aiLines = @()
    $handicapLines = @()
    $countryLines = @()
    $colorLines = @()
    if ($AI -gt 0) {
        $countries = if ($AICountries) { @($AICountries.Split(',') | ForEach-Object { $_.Trim() } | Where-Object { $_ }) } else { @('8', '9', '5', '6', '7', '0') }
        for ($n = 0; $n -lt $AI; $n++) {
            $multi = "Multi$($n + 3)"
            $handicapLines += "$multi=1"
            $countryLines += "$multi=$($countries[$n])"
            $colorLines += "$multi=$($n + 2)"
        }
    }
    $handicapBlock = if ($handicapLines) { "`r`n[HouseHandicaps]`r`n" + ($handicapLines -join "`r`n") + "`r`n" } else { '' }
    $countryBlock = if ($countryLines) { "`r`n[HouseCountries]`r`n" + ($countryLines -join "`r`n") + "`r`n" } else { '' }
    $colorBlock = if ($colorLines) { "`r`n[HouseColors]`r`n" + ($colorLines -join "`r`n") + "`r`n" } else { '' }
    $tunnelBlock = if ($BridgePort -gt 0) { "`r`n[Tunnel]`r`nIp=127.0.0.1`r`nPort=$BridgePort`r`n" } else { '' }
    # The client writes no BindAddress. A direct copy gets one anyway, so it listens on loopback only.
    $bindLine = if ($BridgePort -gt 0) { '' } else { "BindAddress=127.0.0.1`r`n" }
@"
[Settings]
Name=$Name
Scenario=$Map
UIGameMode=Standard
UIMapName=$mapBase
PlayerCount=2
Side=$Side
IsSpectator=False
Color=$Color
CustomLoadScreen=
AIPlayers=$AI
$modeLine
Seed=$Seed
FrameSendRate=$cncnetFrameSendRate
$cncnetMaxAheadLine
Protocol=$cncnetProtocol
GameID=1
Host=$HostFlag
Port=$Port
$bindLine
Credits=10000
UnitCount=0
Bases=True
GameSpeed=$GameSpeed
ShortGame=False
EnableReplayRecording=False
GamePackageVersion=
BroadcastedGameOptionValues=
MapSHA1=

[Other1]
Name=$PeerName
Side=$PeerSide
IsSpectator=False
Color=$PeerColor
Ip=$PeerIp
Port=$PeerPort
$handicapBlock$countryBlock$colorBlock$tunnelBlock
"@
}

# Side and colour of each copy. -HostLast puts the host (side A) in the higher seat.
$sideA = if ($HostLast) { 8 } else { 0 }
$colourA = if ($HostLast) { 1 } else { 0 }
$sideB = if ($HostLast) { 0 } else { 8 }
$colourB = if ($HostLast) { 0 } else { 1 }

if ($SpawnStyle -eq 'CnCNet') {
    $iniA = New-CnCNetSpawnIni 'SyncA' $sideA $colourA $(if ($Tunnel) { $TunnelIdA } else { $PortA }) 'yes' 'SyncB' $sideB $colourB $(if ($Tunnel) { $TunnelIdB } else { $PortB }) $(if ($Tunnel) { '0.0.0.0' } else { '127.0.0.1' }) $(if ($Tunnel) { $BridgePortA } else { 0 })
    $iniB = New-CnCNetSpawnIni 'SyncB' $sideB $colourB $(if ($Tunnel) { $TunnelIdB } else { $PortB }) 'no' 'SyncA' $sideA $colourA $(if ($Tunnel) { $TunnelIdA } else { $PortA }) $(if ($Tunnel) { '0.0.0.0' } else { '127.0.0.1' }) $(if ($Tunnel) { $BridgePortB } else { 0 })
} elseif ($Tunnel) {
    $iniA = New-SpawnIni 'SyncA' $sideA $colourA $TunnelIdA 'yes' 'SyncB' $sideB $colourB $TunnelIdB $BridgePortA
    $iniB = New-SpawnIni 'SyncB' $sideB $colourB $TunnelIdB 'no' 'SyncA' $sideA $colourA $TunnelIdA $BridgePortB
} else {
    $iniA = New-SpawnIni 'SyncA' $sideA $colourA $PortA 'yes' 'SyncB' $sideB $colourB $PortB
    $iniB = New-SpawnIni 'SyncB' $sideB $colourB $PortB 'no' 'SyncA' $sideA $colourA $PortA
}

# The CnCNet style turns the sync log on here, since the client passes only -SPAWN.
$syncLogLine = if ($SpawnStyle -eq 'CnCNet') { 'SyncLog=yes' } else { '' }
$muted = @"
[Intro]
Play=no

[Options]
GameSpeed=$GameSpeed
MenuStyle=Modern
InterfaceStyle=$InterfaceStyle
ReportUpload=never
$syncLogLine

[Video]
ScreenWidth=$Width
ScreenHeight=$Height
Fullscreen=no
StretchMovies=no
RenderFrameRate=0
WindowWidth=0
WindowHeight=0

[Audio]
SoundVolume=0.000000
VoiceVolume=0.000000
ScoreVolume=0.000000
InGameMusic=no
"@

# Writes the copy's script: a log line at frame 0 that names the copy, then the copy's own script.
function New-SideScript([string]$path, [string]$side, [string]$source) {
    $lines = @("0 log NETSYNC_SIDE_$side")
    if ($source) { $lines += Get-Content -LiteralPath $source }
    Set-Content -LiteralPath $path -Value $lines -Encoding ascii
}

foreach ($user in $userList) {
    Set-Content -Encoding ascii (Join-Path $user 'RA2MD.INI') $muted
}
function Add-ColorRGB([string]$Ini, [string]$Own, [string]$Peer) {
    if ($Own) { $Ini = $Ini -replace '(\[Settings\]\r?\n)', ('$1ColorRGB=' + $Own + [Environment]::NewLine) }
    if ($Peer) { $Ini = $Ini -replace '(\[Other1\]\r?\n)', ('$1ColorRGB=' + $Peer + [Environment]::NewLine) }
    return $Ini
}
$iniA = Add-ColorRGB $iniA $ColorRGBA $ColorRGBB
$iniB = Add-ColorRGB $iniB $ColorRGBB $ColorRGBA
if ($ThirdSeat) {
    # Side C joins as the third seat. A and B each list it as [Other2]; C lists B as [Other2].
    $sideC = 4
    $colourC = 2
    $iniA += "`r`n[Other2]`r`nName=SyncC`r`nSide=$sideC`r`nColor=$colourC`r`nIp=127.0.0.1`r`nPort=$PortC`r`n"
    $iniB += "`r`n[Other2]`r`nName=SyncC`r`nSide=$sideC`r`nColor=$colourC`r`nIp=127.0.0.1`r`nPort=$PortC`r`n"
    $iniC = New-SpawnIni 'SyncC' $sideC $colourC $PortC 'no' 'SyncA' $sideA $colourA $PortA
    $iniC += "`r`n[Other2]`r`nName=SyncB`r`nSide=$sideB`r`nColor=$colourB`r`nIp=127.0.0.1`r`nPort=$PortB`r`n"
    Set-Content -Encoding ascii (Join-Path $userC 'SPAWN.INI') $iniC
}
Set-Content -Encoding ascii (Join-Path $userA 'SPAWN.INI') $iniA
Set-Content -Encoding ascii (Join-Path $userB 'SPAWN.INI') $iniB

$scriptPathA = Join-Path $userA 'netsync-side.txt'
$scriptPathB = Join-Path $userB 'netsync-side.txt'
New-SideScript $scriptPathA 'A' $ScriptA
New-SideScript $scriptPathB 'B' $ScriptB
if ($ThirdSeat) {
    $scriptPathC = Join-Path $userC 'netsync-side.txt'
    New-SideScript $scriptPathC 'C' $ScriptC
}

# Old logs go, so that only this run's logs remain.
Remove-Item (Join-Path $logs 'DEBUG_*.LOG') -ErrorAction SilentlyContinue
Remove-Item (Join-Path $logs 'SYNC_*.LOG') -ErrorAction SilentlyContinue

function New-GameArgs([string]$user, [string]$script, [int]$forceDesync = 0) {
    # The CnCNet style adds only the harness switches; its sync log comes from RA2MD.INI.
    $gameArgs = @("-DATADIR=`"$DataDir`"", "-USERDIR=`"$user`"", '-SPAWN', '-WIN', '-MULTIINSTANCE',
      "-AUTOTEST=`"$script`"")
    if ($SpawnStyle -eq 'Runner') { $gameArgs += '-SYNCLOG' }
    if ($forceDesync -gt 0) { $gameArgs += "-DESYNCTEST=$forceDesync" }
    return $gameArgs
}

"netsync: $exe, map $Map, seed $Seed, $(if ($ThirdSeat) { 3 } else { 2 }) people + $AI computer players, ports $PortA and $PortB$(if ($ThirdSeat) { " and $PortC" })"
"netsync: spawn style $SpawnStyle$(if ($Tunnel) { "; tunnel through $RelayPort, bridges $BridgePortA and $BridgePortB, ids $TunnelIdA and $TunnelIdB" } else { '' })"

# The relay runs with -I, as python's isolated mode, and is stopped by its process ID only. Its
# stats and its final summary go to a log beside the runner's copy folders.
$relayProc = $null
$relayLog = Join-Path $WorkDir 'relay.log'
if ($Tunnel) {
    Remove-Item $relayLog, "$relayLog.err" -ErrorAction SilentlyContinue
    # Start-Process joins the arguments without quotes, so the script path is quoted here.
    $relayArgs = @('-I', "`"$relayScript`"", '--relay-port', "$RelayPort",
        '--player', "$($TunnelIdA):$BridgePortA", '--player', "$($TunnelIdB):$BridgePortB",
        '--latency-ms', "$LatencyMs", '--jitter-ms', "$JitterMs", '--loss', "$LossPercent", '--seed', "$RelaySeed",
        '--relay-buffer', "$RelayBuffer",
        '--duration', "$($Seconds + 3)", '--stats-interval', '30')
    $relayProc = Start-Process -FilePath 'python' -ArgumentList $relayArgs -WorkingDirectory $root -PassThru `
        -WindowStyle Hidden -RedirectStandardOutput $relayLog -RedirectStandardError "$relayLog.err"
    $readyDeadline = (Get-Date).AddSeconds(15)
    $relayReady = $false
    while (-not $relayReady -and (Get-Date) -lt $readyDeadline -and -not $relayProc.HasExited) {
        if (Test-Path -LiteralPath $relayLog) {
            $relayReady = [bool](Select-String -LiteralPath $relayLog -Pattern 'relay: ready' -Quiet)
        }
        if (-not $relayReady) { Start-Sleep -Milliseconds 200 }
    }
    if (-not $relayReady) {
        if (-not $relayProc.HasExited) { Stop-Process -Id $relayProc.Id -Force -ErrorAction SilentlyContinue }
        "netsync: the relay did not start; see $relayLog and $relayLog.err"
        exit 1
    }
    "netsync: relay pid $($relayProc.Id) listens on 127.0.0.1:$RelayPort; latency $LatencyMs ms, jitter $JitterMs ms, loss $LossPercent%"
}

$start = Get-Date
$procA = Start-Process -FilePath $exe -WorkingDirectory $bin -PassThru -ArgumentList (New-GameArgs $userA $scriptPathA)
$procB = Start-Process -FilePath $exe -WorkingDirectory $bin -PassThru -ArgumentList (New-GameArgs $userB $scriptPathB $ForceDesyncB)
if ($ForceDesyncB -gt 0) { "netsync: side B checksum is corrupted on frame $ForceDesyncB (positive control)" }
"netsync: started side A (pid $($procA.Id)) and side B (pid $($procB.Id))"

$procC = $null
if ($ThirdSeat) {
    $procC = Start-Process -FilePath $exe -WorkingDirectory $bin -PassThru -ArgumentList (New-GameArgs $userC $scriptPathC $ForceDesyncC)
    if ($ForceDesyncC -gt 0) { "netsync: side C checksum is corrupted on frame $ForceDesyncC (positive control)" }
    "netsync: started side C (pid $($procC.Id))"
}
$procs = @($procA, $procB)
if ($procC) { $procs += $procC }

# Shows the address each copy listens on, so a run can confirm loopback only.
function Get-Bound([int]$procId) {
    try {
        $ends = @(Get-NetUDPEndpoint -OwningProcess $procId -ErrorAction Stop)
        if ($ends.Count -eq 0) { return 'no UDP endpoint yet' }
        return (($ends | ForEach-Object { "$($_.LocalAddress):$($_.LocalPort)" }) -join ', ')
    } catch {
        return 'unknown'
    }
}

$deadline = $start.AddSeconds($Seconds)
$checked = $false
while ((Get-Date) -lt $deadline -and @($procs | Where-Object { -not $_.HasExited }).Count -gt 0) {
    if (-not $checked -and (Get-Date) -gt $start.AddSeconds(10)) {
        "netsync: side A listens on $(Get-Bound $procA.Id)"
        "netsync: side B listens on $(Get-Bound $procB.Id)"
        if ($procC) { "netsync: side C listens on $(Get-Bound $procC.Id)" }
        $checked = $true
    }
    Start-Sleep -Milliseconds 500
}

$exits = @()
foreach ($p in $procs) {
    if (-not $p.HasExited) {
        Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue
        $p.WaitForExit(10000) | Out-Null
        $exits += "stopped after $Seconds s"
    } else {
        $exits += "exited with $($p.ExitCode)"
    }
}
"netsync: side A $($exits[0]); side B $($exits[1])"
if ($procC) { "netsync: side C $($exits[2])" }

# The relay stops itself after -Seconds plus three; the runner waits for its summary, then stops it
# by PID if it is still running.
if ($relayProc) {
    if (-not $relayProc.HasExited) { $relayProc.WaitForExit(20000) | Out-Null }
    if (-not $relayProc.HasExited) {
        Stop-Process -Id $relayProc.Id -Force -ErrorAction SilentlyContinue
        $relayProc.WaitForExit(5000) | Out-Null
        "netsync: relay pid $($relayProc.Id) did not stop by itself and was stopped; no summary"
    } else {
        $relayProc.WaitForExit() | Out-Null
        "netsync: relay pid $($relayProc.Id) exited with $($relayProc.ExitCode)"
    }
    $relaySummary = Select-String -LiteralPath $relayLog -Pattern 'relay: (stopped|game datagram lengths)' -ErrorAction SilentlyContinue
    foreach ($line in @($relaySummary)) { "netsync: $($line.Line.Trim())" }
}

# Finds each copy's debug log by the marker line its script wrote at frame 0.
function Find-Log([string]$side) {
    $candidates = Get-ChildItem (Join-Path $logs 'DEBUG_*.LOG') -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending
    foreach ($f in $candidates) {
        if (Select-String -LiteralPath $f.FullName -Pattern "NETSYNC_SIDE_$side" -Quiet) { return $f.FullName }
    }
    return $null
}

# Reads a log while the game may still hold it open for writing. Plain ReadLines refuses that.
function Read-SharedLines([string]$path) {
    $lines = New-Object System.Collections.Generic.List[string]
    $share = [System.IO.FileShare]::ReadWrite -bor [System.IO.FileShare]::Delete
    $stream = [System.IO.File]::Open($path, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, $share)
    try {
        $reader = New-Object System.IO.StreamReader($stream)
        while (-not $reader.EndOfStream) { $lines.Add($reader.ReadLine()) }
    } finally {
        $stream.Dispose()
    }
    return [string[]]$lines.ToArray()
}

function Read-Ticks([string]$path) {
    $ticks = @{}
    if (-not $path) { return $ticks }
    foreach ($line in (Read-SharedLines $path)) {
        if ($line -match 'SYNC frame (\d+) crc ([0-9a-fA-F]{8})') {
            $ticks[[int]$Matches[1]] = $Matches[2].ToLowerInvariant()
        }
    }
    return $ticks
}

function Get-Reports([string]$path) {
    if (-not $path) { return @() }
    $found = @()
    foreach ($line in (Read-SharedLines $path)) {
        if ($line -match 'Writing out-of-sync report to (.+)$') { $found += $Matches[1].Trim() }
    }
    return $found
}

$logA = Find-Log 'A'
$logB = Find-Log 'B'
$ticksA = Read-Ticks $logA
$ticksB = Read-Ticks $logB

function Describe-Ticks([string]$side, [hashtable]$ticks, [string]$log) {
    if (-not $log) { return "side ${side}: no debug log found with the NETSYNC marker" }
    if ($ticks.Count -eq 0) { return "side ${side}: log $log has no SYNC lines; the match did not start" }
    $frames = @($ticks.Keys | Sort-Object)
    return "side ${side}: $($ticks.Count) ticks, frames $($frames[0])-$($frames[-1]), log $log"
}
Describe-Ticks 'A' $ticksA $logA
Describe-Ticks 'B' $ticksB $logB
if ($ThirdSeat) {
    $logC = Find-Log 'C'
    $ticksC = Read-Ticks $logC
    Describe-Ticks 'C' $ticksC $logC
}

# Reports from the game: a desync dialog or an out-of-sync report on either side.
$outOfSync = @()
foreach ($pair in @(@('A', $logA), @('B', $logB))) {
    if (-not $pair[1]) { continue }
    $hits = @(Select-String -LiteralPath $pair[1] -Pattern 'has gone out of sync on frame (\d+)' -AllMatches)
    foreach ($h in $hits) {
        $outOfSync += "side $($pair[0]): $($h.Line.Trim())"
    }
}
$reportFiles = @()
foreach ($pair in @(@('A', $logA), @('B', $logB))) {
    foreach ($r in (Get-Reports $pair[1])) { $reportFiles += "side $($pair[0]): $r" }
}
if ($outOfSync.Count -gt 0) {
    "netsync: the game reported an out-of-sync match:"
    $outOfSync | Select-Object -First 6 | ForEach-Object { "  $_" }
}
foreach ($r in $reportFiles) { "netsync: out-of-sync report $r" }

# The first differing lines of the two reports, from the object sections on. Each report begins
# with per-machine lines (local house, CPU, FPS) that always differ, so they are skipped.
function Compare-Reports([string]$pathA, [string]$pathB) {
    $linesA = Read-SharedLines $pathA
    $linesB = Read-SharedLines $pathB
    $marker = '----- Houses -----'
    $startA = [array]::IndexOf($linesA, $marker)
    $startB = [array]::IndexOf($linesB, $marker)
    if ($startA -lt 0 -or $startB -lt 0) { return @('no houses section in one report') }
    $differences = @()
    $count = [math]::Min($linesA.Count - $startA, $linesB.Count - $startB)
    for ($i = 0; $i -lt $count; $i++) {
        $la = $linesA[$startA + $i]
        $lb = $linesB[$startB + $i]
        if ($la -ne $lb) {
            $differences += "  line $($i + 1) of the object sections"
            $differences += "    A: $la"
            $differences += "    B: $lb"
            if ($differences.Count -ge 30) { break }
        }
    }
    if ($differences.Count -eq 0 -and $linesA.Count -ne $linesB.Count) {
        $differences += "  the object sections differ in length"
    }
    return $differences
}
if ($reportFiles.Count -ge 2) {
    $pathA = ($reportFiles | Where-Object { $_ -like 'side A:*' } | Select-Object -First 1) -replace '^side A: ', ''
    $pathB = ($reportFiles | Where-Object { $_ -like 'side B:*' } | Select-Object -First 1) -replace '^side B: ', ''
    if ($pathA -and $pathB -and (Test-Path -LiteralPath $pathA) -and (Test-Path -LiteralPath $pathB)) {
        "netsync: first differences between the two out-of-sync reports:"
        Compare-Reports $pathA $pathB | ForEach-Object { $_ }
    }
}

# Compares the two sides on the frames both logged.
$common = @($ticksA.Keys | Where-Object { $ticksB.ContainsKey($_) } | Sort-Object)
$firstMismatch = $null
foreach ($f in $common) {
    if ($ticksA[$f] -ne $ticksB[$f]) { $firstMismatch = $f; break }
}

if ($null -ne $firstMismatch) {
    "netsync: first mismatching tick $firstMismatch (side A $($ticksA[$firstMismatch]), side B $($ticksB[$firstMismatch]))"
}
if ($common.Count -gt 0) {
    "netsync: compared $($common.Count) common ticks, frames $($common[0])-$($common[-1])"
}

if ($null -ne $firstMismatch -or $outOfSync.Count -gt 0) {
    "RESULT: DESYNC"
    $exitCode = 3
} elseif ($common.Count -eq 0) {
    "RESULT: NO COMMON TICKS; the match did not run"
    $exitCode = 4
} else {
    "RESULT: IN SYNC over $($common.Count) ticks"
    $exitCode = 0
}

if ($KeepDir) {
    New-Item -ItemType Directory -Force $KeepDir | Out-Null
    $stamp = $start.ToString('yyyyMMdd-HHmmss')
    if ($logA) { Copy-Item $logA (Join-Path $KeepDir "netsync-$stamp-A.log") -Force; "netsync: kept $(Join-Path $KeepDir "netsync-$stamp-A.log")" }
    if ($logB) { Copy-Item $logB (Join-Path $KeepDir "netsync-$stamp-B.log") -Force; "netsync: kept $(Join-Path $KeepDir "netsync-$stamp-B.log")" }
    if ($Tunnel -and (Test-Path -LiteralPath $relayLog)) { Copy-Item $relayLog (Join-Path $KeepDir "netsync-$stamp-relay.log") -Force; "netsync: kept $(Join-Path $KeepDir "netsync-$stamp-relay.log")" }
    foreach ($r in $reportFiles) {
        $path = $r -replace '^side [AB]: ', ''
        if (Test-Path -LiteralPath $path) {
            $dest = Join-Path $KeepDir ("netsync-$stamp-" + (Split-Path $path -Leaf))
            Copy-Item $path $dest -Force
            "netsync: kept $dest"
        }
    }
}

} catch {
    "netsync: runner error: $($_.Exception.Message)"
    $exitCode = 1
} finally {
    if ($relayProc -and -not $relayProc.HasExited) { Stop-Process -Id $relayProc.Id -Force -ErrorAction SilentlyContinue }
    foreach ($m in $held) { $m.ReleaseMutex() }
    $gameSlots | ForEach-Object { $_.Dispose() }
    $buildLock.ReleaseMutex()
    $buildLock.Dispose()
}

exit $exitCode
