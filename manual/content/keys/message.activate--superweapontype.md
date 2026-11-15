---
key: Message.Activate
scope: superweapontype
summary: "Prints a message to every player when the effect starts."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A string table label, printed when the effect starts, after any [`SW.Deferment=`](/keys/sw.deferment/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.Activate=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
