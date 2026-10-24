---
title: Host and join CnCNet games from the main menu
category: feature
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The main menu has a CnCNet (experimental) entry that opens a lobby inside OpenYR. It connects to the CnCNet chat network, lists the games announced for Yuri's Revenge, and hosts or joins a game room. A room sets side, colour, team and start position, or Spectator, for each player. The host's map and options reach every guest, and a guest whose map is missing or differs from the host's cannot get ready. When the host starts, every player's copy starts the match in a second window and the match plays through a CnCNet tunnel. After the match every player is back in the room.

The CnCNet admins agreed to OpenYR testing on the live network on these terms: an OpenYR game looks like an ordinary Yuri's Revenge game, with the stock game id, channels and version, and the host tells anyone who joins that it is an OpenYR game. The host's room says so once for every player who joins, in the text of `RoomNotice`, and the room screen reminds the host. A stock client never answers OpenYR's hello. The room lists it as not OpenYR from the moment it joins, and removes it 20 seconds later if no hello has come; the host cannot start the match while it is listed. Only play with other OpenYR players on the same build. A password is optional, as in the stock client.

The host's copy picks the tunnel. It downloads the CnCNet tunnel master list, asks each official and recommended version 3 tunnel to answer a small ping, and takes the nearest one with room for the match. The list is kept in `cncnet_tunnels.txt` in the user folder and read when a download fails. `Tunnel=` names a tunnel to use instead.

The lobby is gentle with the chat servers. It sends at most one line every 2.5 seconds after a burst of three, and answers the server's pings. After a lost connection it waits 4 seconds before it tries again, then 8, 16, 32 and 60, and it gives up after 6 attempts in a row that fail or last less than a minute. A server message that names a ban or a throttle stops it at once. The lobby then shows why, and opening the lobby again starts afresh.

A match plays through one tunnel server for all players. Each player's tunnel number is the game's 16-bit number plus 16 more bits that the match draws, so other matches on the same tunnel do not collide with it.

The lobby reads these keys in the `[CnCNet]` section of the settings file:

| Key | Default | Meaning |
| --- | --- | --- |
| `NativeLobby` | `yes` | Shows the main menu entry. |
| `AllowLiveServers` | `yes` | Lets the lobby reach servers that are not on this machine. With `no` only `127.0.0.1` servers and tunnels are used. |
| `Servers` | the CnCNet IRC servers | `host:port,port;host:port` list. |
| `Nick` | the multiplayer handle | The chat nickname. |
| `GameID` | `yr` | The game id in the chat ident. |
| `GameName` | `YR` | The game name in the registration text. |
| `ClientVersion` | `9.3.5` | The version in the registration text and the game list. |
| `ChatChannel` | `#cncnet-yr` | The lobby chat channel. |
| `BroadcastChannel` | `#cncnet-yr-games` | The channel where games are announced. |
| `RoomPrefix` | `#cncnet-yr-game` | The start of a room channel's name. |
| `RoomNotice` | the OpenYR notice | The line said to each player who joins a hosted room; empty says nothing. |
| `Tunnel` | empty | `host:port` of a tunnel to use whatever the list says. Empty picks the nearest from the list. |
| `TunnelListURL` | `https://cncnet.org/master-list` | Where the tunnel list is downloaded. |
| `HostMap` | empty | A map file a hosted room starts on. |
| `MatchArguments` | empty | Added to the command line of the copy that plays the match. |
| `TimingPolicy` | `Adaptive` | The network timing the match plays when this player hosts. `Gamemd` plays gamemd's online timing. |

The lobby chat channel needs a key that OpenYR does not have, so it does not join it and the lobby shows the game list and the global channel.
