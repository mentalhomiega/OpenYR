---
key: AllyChangeAllowed
scope: global-rules
label: Alliances can change during a match
summary: Whether a player may make or break an alliance during a multiplayer match.
see_also: [AlliesAllowed]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rulesmd.ini"
[MultiplayerDialogSettings]
AllyChangeAllowed=no
```

With `no`, the alliance command does nothing during a match, so the teams chosen in the lobby stay as they are. With `yes`, a player can ally with or leave another human player as usual, when the match allows alliances at all.

The setting has no effect in a campaign mission.
