---
key: Light.Blue
scope: superweapontype
summary: "Sets the blue tint of the lighting a nuclear missile, lightning storm or psychic dominator gives the map."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The scenario's IonBlue or DominatorBlue (storm, dominator); 125 (missile)"
---

While the weapon's effect shows, the map's blue tint is this percentage, where 100 leaves the color alone. A negative value keeps the tint the effect uses without the key. Only `Type=MultiMissile`, `Type=LightningStorm` and `Type=PsychicDominator` weapons read it, and `Light.Enabled=no` turns it off.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Light.Blue=100
```

See [Lighting](/systems/superweapons/#lighting).
