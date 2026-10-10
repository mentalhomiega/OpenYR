---
key: WantsExtraSpace
scope: buildingtype
label: Wants extra room from the computer
summary: Makes a computer house leave one more cell of free ground around this structure when it places it.
see_also: [ProtectWithWall, AIBaseSpacing, "system:ai-base-building"]
when_omitted:
  kind: value
  value: "no"
---

The flag adds one cell to the border that a computer house's [placement search](/systems/ai-base-building/#the-placement-search) requires around the structure. [`ProtectWithWall=yes`](/keys/protectwithwall/) has the same effect. The first try uses [`AIBaseSpacing`](/keys/aibasespacing/) plus one cells of border, and the second try uses one cell. The search also starts its look one cell further out from the base on the sides that face south.

```ini title="rulesmd.ini"
[YAWEAP]
WantsExtraSpace=yes
```

Only a computer house reads the flag. The shipped rules leave the key commented out.
