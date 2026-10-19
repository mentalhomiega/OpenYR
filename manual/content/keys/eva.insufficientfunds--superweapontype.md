---
key: EVA.InsufficientFunds
scope: superweapontype
summary: "Names the announcer line played when a shot is refused for lack of money."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "EVA_InsufficientFunds"
---

An announcer event from the EVA file, played to the player who fired. `none` plays nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EVA.InsufficientFunds=none
```

See [Announcements](/systems/superweapons/#announcements).
