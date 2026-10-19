---
key: Message.InsufficientFunds
scope: superweapontype
summary: "Prints a message when a shot is refused for lack of money."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A string table label, printed to the player who fired.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.InsufficientFunds=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
