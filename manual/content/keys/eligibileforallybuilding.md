---
key: EligibileForAllyBuilding
summary: Whether an ally's placements may anchor on a building of this type.
see_also: [BaseNormal, ConstructionYard, "system:base-adjacency"]
when_omitted:
  kind: value
  value: "no"
---

`EligibileForAllyBuilding=yes` lets a building of this type anchor a placement by a player its owner counts as an ally, when the match allows [building off an ally](/systems/base-adjacency/#building-off-an-ally). Without that match option the key has no effect.

```ini title="rules.ini"
[GAPOWR]
EligibileForAllyBuilding=yes ; an ally's placements can anchor on a GAPOWR
```

Only the alliance held by a building's owner is tested. The building anchors when its owner counts the placing player as an ally, even if the placing player does not count its owner. For an ally's placement, the building's [`BaseNormal`](/keys/basenormal/) value is not checked, so this key alone decides. A player's own buildings still anchor by `BaseNormal` and ignore this key.

The stock rules set this key only on the construction yards `GACNST` and `NACNST`. With the stock rules and building off an ally on, allies can place next to each other's construction yards and no other structure. [`ConstructionYard=yes`](/keys/constructionyard/) does not set this key, so a construction yard type without it does not anchor an ally's placement.
