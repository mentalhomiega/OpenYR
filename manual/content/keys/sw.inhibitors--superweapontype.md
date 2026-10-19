---
key: SW.Inhibitors
scope: superweapontype
summary: "Blocks this superweapon near enemy objects of the listed types."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A cell is not a valid target while a live object of a listed type, owned by a house that is neither the firing house nor its ally, stands within its [`InhibitorRange=`](/keys/inhibitorrange/) of the cell. `InhibitorRange=` defaults to the object's `Sight=`. A structure inhibits only while it is powered on. A blocked cell shows [`NoCursor=`](/keys/nocursor/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Inhibitors=FAINHI,FAJAMM
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
