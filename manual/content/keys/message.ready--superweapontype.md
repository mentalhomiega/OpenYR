---
key: Message.Ready
scope: superweapontype
summary: "Prints a message to the owner when the weapon is charged."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A string table label.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.Ready=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
