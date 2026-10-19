---
key: SW.AnyInhibitor
scope: superweapontype
summary: "Lets any enemy object inhibit this superweapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, any live enemy object within its `InhibitorRange=` of the cell inhibits the shot, whatever its type. The key replaces the [`SW.Inhibitors=`](/keys/sw.inhibitors/) list.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AnyInhibitor=yes
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
