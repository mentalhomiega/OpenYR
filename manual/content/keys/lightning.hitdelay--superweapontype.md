---
key: Lightning.HitDelay
scope: superweapontype
summary: "Sets how often a cloud gathers over the center of a lightning storm."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningHitDelay"
---

While a `Type=LightningStorm` weapon's storm rages, a cloud gathers over the target cell every this many frames. A value of 0 or less gathers none there. Clouds over the center ignore [`Lightning.Separation=`](/keys/lightning.separation/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.HitDelay=60
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
