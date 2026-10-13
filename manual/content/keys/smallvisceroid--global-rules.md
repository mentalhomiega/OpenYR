---
key: SmallVisceroid
scope: global-rules
label: Named visceroid type
see_also: ["TiberiumDeathToVisceroid", "LargeVisceroid", "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

The named UnitType is read and kept, but nothing spawns it. No death leaves a visceroid, so the key has no effect on play.

```ini title="rules.ini"
[General]
SmallVisceroid=VISC_SML  ; the UnitType the shipped rules name
```

Naming a type here does not make it behave like a visceroid. Merging, wandering and the creature artwork come from [`SmallVisceroid=yes`](/keys/smallvisceroid/#scope-unittype) in that type's own section. Without it, the named type is an ordinary vehicle.

The name does not have to match a declared UnitType. A name that matches no type creates a UnitType of that name, which reads its statistics from a section of the same name and stays blank if the rules have none. Only `none` and `<none>` select no type.
