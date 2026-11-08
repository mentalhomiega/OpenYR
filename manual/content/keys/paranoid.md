---
key: Paranoid
summary: Whether the computer houses band together against the human players when the game turns against them.
see_also: [MaxIQLevels, IQ]
when_omitted:
  kind: value
  value: "yes"
---

The computer houses band together against the human players only when [`AlliesAllowed=yes`](/keys/alliesallowed/) is set in `[MultiplayerDialogSettings]` or the game is a Cooperative match, and no computer player started the game with allies. The stock rules set `AlliesAllowed=no`, so in a stock game that is not Cooperative they never band together. When they do, every computer house still in the game allies with every other computer house still in the game, and declares war on every surviving human house. Each alliance this breaks is broken on both sides. Each declaration also adds to the computer house's anger at that human house, which steers the computer's choice of enemy.

Outside a campaign, a [`MultiplayPassive=yes`](/keys/multiplaypassive/) house such as Neutral takes no part in the war declarations. An alliance is also refused when it would leave the computer house with no enemy left in the game.

Three events set it off, all outside a campaign:

- With this switch on, a human house forms an alliance with any house that is not [`MultiplayPassive=yes`](/keys/multiplaypassive/), a computer house included. Alliances made while the scenario is being set up, such as those a map or launch file declares, count only in an Internet game.
- With this switch on, a computer house is defeated while its [`IQ`](/keys/iq/) equals [`MaxIQLevels`](/keys/maxiqlevels/). Every computer player in a skirmish or multiplayer game has that IQ.
- A computer house deploys an MCV into a construction yard, whatever this switch says.

Playing back a recording is one more case. When a recorded player leaves the game, the playback hands that house to the computer and makes the computer houses band together at once, whatever this switch says, provided `AlliesAllowed` allows it.
