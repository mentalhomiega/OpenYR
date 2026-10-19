---
key: SW.Designators
scope: superweapontype
summary: "Requires one of the player's objects of the listed types near the target."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A cell is a valid target only when the firing house or an ally owns a live object of a listed type within its [`DesignatorRange=`](/keys/designatorrange/) of the cell. `DesignatorRange=` defaults to the object's `Sight=`. An empty value or `none` needs no designator.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Designators=GAPOST
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
