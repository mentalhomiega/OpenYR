---
title: Choose the multiplayer mode in a launch file
category: feature
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- MentalHomiega
---

A launch file can name a multiplayer mode with `GameMode`, the mode's index in the game's `MPModesMD.ini`. Without the key, Battle plays, and the match plays as before.

FreeForAll and Cooperative turn `AlliesAllowed` off, so players cannot change alliances during play. UnholyAlliance turns bases on and gives each house every base unit the rules list. In Cooperative, the local player wins when the last house still alive is a person, even if the local player was defeated.

A new skirmish or game against other machines is refused when `MPModesMD.ini` cannot be read or `GameMode` names no mode in it.

The session identity now includes the launch file's schema version, which is 6. A machine on an earlier build does not share a match with this one.
