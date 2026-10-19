---
key: Message.Launch
scope: superweapontype
summary: "Prints a message to every player when the weapon fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A string table label, printed at the launch before any [`SW.Deferment=`](/keys/sw.deferment/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.Launch=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
