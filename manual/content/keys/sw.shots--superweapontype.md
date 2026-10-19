---
key: SW.Shots
scope: superweapontype
summary: "Limits how many times each house may fire this superweapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "-1"
---

A negative value allows unlimited shots. With `SW.Shots=2`, a house that has fired the weapon twice no longer holds it, even while it owns the structure that grants it. Every launch counts, including one a trigger granted.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Shots=2
```

See [Availability rules](/systems/superweapons/#availability-rules).
