---
key: Deliver.Types
scope: superweapontype
summary: "Lists the objects a unit delivery brings in."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

Each listed vehicle, infantry, aircraft or structure type is created at the target, owned by [`Deliver.Owner=`](/keys/deliver.owner/). A type listed twice arrives twice. Each object takes the first free cell around the target, tried in widening rings up to six cells out, and one that finds no room is dropped. Delivered vehicles, infantry and aircraft go on area guard when a house a person controls owns them, and hunt otherwise.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Deliver.Types=E1,E1,E2
```

See [Unit delivery](/systems/superweapons/#unit-delivery).
