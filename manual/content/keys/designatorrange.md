---
key: DesignatorRange
summary: "Radius in cells within which the object counts as a designator for a superweapon's [`SW.Designators=`](/keys/sw.designators/) list."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The type's Sight."
---

Radius in cells within which the object counts as a designator for a superweapon's [`SW.Designators=`](/keys/sw.designators/) list. A negative value, or a missing key, uses the type's [`Sight`](/keys/sight/). The range counts whole cells from the object's center cell to the target cell.

```ini title="rules.ini"
[MYTOWER] ; example BuildingType
DesignatorRange=6
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
