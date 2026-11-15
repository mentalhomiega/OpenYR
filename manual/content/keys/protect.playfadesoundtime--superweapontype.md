---
key: Protect.PlayFadeSoundTime
scope: superweapontype
summary: "Sets how many frames before a force shield ends its `SpecialSound=` plays."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] ForceShieldPlayFadeSoundTime"
---

A `Type=ForceShield` weapon plays its [`SpecialSound=`](/keys/specialsound/) this many frames before the protection ends. Keep the value below [`Protect.Duration=`](/keys/protect.duration/). A negative value, or a missing key, uses the rules value.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Protect.PlayFadeSoundTime=50
```

See [Force shield](/systems/superweapons/#force-shield).
