---
key: Deliver.Owner
scope: superweapontype
summary: "Chooses who owns the objects a unit delivery brings in."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "Invoker"
---

`invoker` is the firing house. `neutral`, `special` and `civilian` name the house with that name in the game. When no such house exists, the firing house owns the objects.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Deliver.Owner=neutral
```

See [Unit delivery](/systems/superweapons/#unit-delivery).
