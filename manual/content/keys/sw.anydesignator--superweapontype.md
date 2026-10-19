---
key: SW.AnyDesignator
scope: superweapontype
summary: "Accepts any of the player's objects as a designator."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, any live object of the firing house or its allies within its `DesignatorRange=` of the cell counts, whatever its type. The key replaces the [`SW.Designators=`](/keys/sw.designators/) list.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AnyDesignator=yes
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
