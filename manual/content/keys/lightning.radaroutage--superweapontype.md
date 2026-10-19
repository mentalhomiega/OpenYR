---
key: Lightning.RadarOutage
scope: superweapontype
summary: "Sets how many frames a lightning storm jams radar."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] LightningStormDuration"
---

When a `Type=LightningStorm` weapon's storm breaks, the houses chosen by [`Lightning.RadarOutageAffects=`](/keys/lightning.radaroutageaffects/) lose their radar for this many frames. A value of 0 or less jams no radar. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.RadarOutage=450
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
