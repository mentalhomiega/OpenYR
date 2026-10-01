---
key: Radar
summary: Whether the structure can supply the player's radar map.
see_also: ["system:power"]
when_omitted:
  kind: value
  value: "no"
---

A `Radar=yes` structure gives the local player the radar map. The map is up while all of these hold:

- no ion storm is running;
- the player's house has at least as much power output as drain;
- the house owns a `Radar=yes` structure that is switched on, on the map and not being sold.

[`FreeRadar=yes`](/keys/freeradar/) replaces the third test, but the storm and power tests still apply. In a campaign, a radar structure the player has not discovered does not count. A player who has been given the whole map keeps the radar whatever the three tests say, as [observers and coach mode](/systems/observers/) describes.

A stunned radar structure can keep the map dark even while another radar structure works, because the house checks only the first structure that passes the tests above. [Radar](/systems/power/#radar) gives the full test.

When a spy enters an enemy's `Radar=yes` structure, the whole map goes back under the shroud and the fog for that enemy, unless it has a working spy satellite. [Infiltrating it](/systems/capture/#infiltrating-it) lists what spies do to other structures.
