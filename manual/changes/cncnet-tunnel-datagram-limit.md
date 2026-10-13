---
title: Let multiplayer games start through a CnCNet tunnel
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

A multiplayer game through a CnCNet tunnel server now starts. The game sends each tunnel datagram in at most 1016 bytes, which the server's 1024-byte buffer holds once the bridge adds its 8-byte envelope. Lobby, chat and in-game messages send only the bytes their message type uses, so most are well under 1016 bytes.

Encoded game options are limited to 900 bytes, down from 960. Longer options text is cut short when the game encodes it. A game with 8 players, long names and a long scenario description can reach that limit.

The session identity changed, so a build from before this change does not agree with this build on a match.
