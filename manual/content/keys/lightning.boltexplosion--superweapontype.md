---
key: Lightning.BoltExplosion
scope: superweapontype
summary: "Names the animation each lightning bolt leaves where it strikes."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The explosion that the bolt's damage and warhead pick"
---

Each bolt of a `Type=LightningStorm` weapon plays this animation on the cell it strikes. Without the key, the bolt plays the explosion that its damage and warhead pick: [`WeatherConBoltExplosion`](/keys/weatherconboltexplosion/) for the stock lightning warhead.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.BoltExplosion=EXPLOLB
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
