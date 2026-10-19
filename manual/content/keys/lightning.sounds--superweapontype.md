---
key: Lightning.Sounds
scope: superweapontype
summary: "Lists the sounds a lightning bolt can play."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[AudioVisual] LightningSounds"
---

Each bolt plays one sound from the list at random. `none`, or a list of names that match no sound, plays no sound.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Sounds=WeatherStrike
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
