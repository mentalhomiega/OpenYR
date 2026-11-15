---
key: Protect.PowerOutage
scope: superweapontype
summary: "Sets how many frames a force shield shuts off its house's power."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] ForceShieldBlackoutDuration"
---

A `Type=ForceShield` weapon blacks out the firing house's power for this many frames. A negative value, or a missing key, uses the rules value. An iron curtain ignores the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Protect.PowerOutage=0
```

See [Force shield](/systems/superweapons/#force-shield).
