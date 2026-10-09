---
key: AIPickWallDefensePercent
scope: global-rules
label: Chance a defense turn becomes a wall ring
summary: Per difficulty slot, the percent chance that a computer house's defense turn rings a protected structure with walls instead.
see_also: [ProtectWithWall, ConcreteWalls, AIExtraRefineries, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. When a computer house's plan reaches a defense turn, it draws a number from `0` to `99`. If the number is below the slot's entry, the turn walls the last structure flagged with [`ProtectWithWall`](/keys/protectwithwall/) that needs walls, instead of placing a defense. The draw happens on every defense turn whether or not the list is set. A slot past the end of the list counts as `0`.

```ini title="rulesmd.ini"
[General]
AIPickWallDefensePercent=50,25,10
```

With no list, or a `0` for the slot, a computer house never walls a structure.
