---
key: AlliesAllowed
summary: Whether the computer houses may band together against the human players in a skirmish or multiplayer game.
see_also: [Paranoid, MaxIQLevels]
when_omitted:
  kind: value
  value: "no"
---

`AlliesAllowed=yes` in `[MultiplayerDialogSettings]` lets the computer houses band together against the human players: every computer house still in the game allies with every other one and declares war on every surviving human house. [`Paranoid`](/keys/paranoid/) lists the events that set this off. With `no`, nothing does. The stock rules set `no`, but a skirmish or network game also reads its [mode's rules file](/formats/spawn-ini/#the-multiplayer-mode), and every stock mode file except FreeForAll's and Cooperative's sets `yes`.

A game in which a computer player starts with allies, as in a team game, never bands the computer houses together, whatever this key says.

```ini title="rulesmd.ini"
[MultiplayerDialogSettings]
AlliesAllowed=yes ; computer houses may band together against the humans
```
