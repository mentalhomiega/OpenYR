---
key: TerrainFire
summary: Lets the attack cursor show over terrain objects and IsARock cells that this weapon would fire at.
see_also: ["TreeTargeting", "IsARock", "LegalTarget"]
when_omitted:
  kind: value
  value: "no"
---

With `TerrainFire=yes`, a player's selected armed object shows the attack cursor over a terrain object when this weapon is the one it would fire at that object. It also shows the cursor over a cell whose overlay sets [`IsARock=yes`](/keys/isarock/), when this weapon is the one it would fire at that cell. A click there orders the attack without the force-fire modifier.

A terrain object takes the cursor in either case: under [`TreeTargeting=yes`](/keys/treetargeting/) or under this key. A cell whose overlay sets `IsARock=yes` takes it only under this key.

The key affects only the player's cursor and click. It does not make terrain objects targets for automatic target scans or for a computer-controlled house.
