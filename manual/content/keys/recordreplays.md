---
key: RecordReplays
summary: Records each skirmish started from a launch file so that it can be played back.
see_also: [PerfLog]
when_omitted:
  kind: value
  value: "no"
---

`RecordReplays=yes` writes a replay of every skirmish that a launch file starts (`-SPAWN`). The file is `<map>-<date>-<time>.rep` in the `Replays` folder of the user directory, and it is written as the match is played, so a match that ends in a crash still leaves a replay that plays up to that point.

A replay holds the build, the checksums of the map, rules, art and AI files, the mods, the random seed, the launch file, every order the match executed with its frame, and a hash of the game state every 30 frames. `-REPLAY=<file>` plays it back from that data, ignores the orders the player gives, and compares the state hashes with the ones recorded.

Campaign missions, network games, resumed saved games and skirmishes started from the menus are not recorded. The setting does not change the match.

A replay plays back only in the build that recorded it, with the same map, rules, art, AI and mods. Any other game refuses it with a message.

```ini title="RA2MD.INI"
[Options]
RecordReplays=yes
```
