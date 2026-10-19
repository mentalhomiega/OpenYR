---
key: Message.FirerColor
scope: superweapontype
summary: "Prints the weapon's messages in the firing house's color."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, [`Message.Detected=`](/keys/message.detected/) and the weapon's other messages use the firing house's color. Otherwise they use the local player's.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.FirerColor=yes
```

See [Announcements](/systems/superweapons/#announcements).
