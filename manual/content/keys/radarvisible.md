---
key: RadarVisible
summary: Whether the object is plotted on the radar map wherever the player has removed the shroud.
see_also: [Invisible, "system:cloaking"]
when_omitted:
  kind: value
  value: "no"
---

`RadarVisible=yes` plots the object on the radar whenever its cell is out of the player's shroud, whatever the ordinary [radar test](/systems/cloaking/#on-the-radar) would decide. Fog of war, cloaking, tunneling, the radar-invisible ability, and whether the player has discovered the object no longer hide it. An object under shroud, a sinking object and one in limbo stay off the radar. An enemy object in revealed cells is plotted from the moment it appears there, before the player has seen it, and stays plotted as it moves through revealed cells.

[`Invisible=yes`](/keys/invisible/) overrides it and keeps the object off the radar for every house. A BuildingType with [`InvisibleInGame=yes`](/keys/invisibleingame/) has `RadarVisible` forced back to `no`.
