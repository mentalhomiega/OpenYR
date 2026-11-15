---
key: Message.Detected
scope: superweapontype
summary: "Prints a message when an enemy builds the structure that grants the weapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A string table label. It prints under the same conditions as [`EVA.Detected=`](/keys/eva.detected/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.Detected=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
