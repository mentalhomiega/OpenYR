---
key: Light.Ambient
scope: superweapontype
summary: "Sets the ambient light of the lighting a nuclear missile, lightning storm or psychic dominator gives the map."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The scenario's IonAmbient (storm), DominatorAmbient (dominator); 200 (missile)"
---

While the weapon's effect shows, the map's ambient light moves to this value, where 100 is full daylight. A negative value keeps the value the effect uses without the key. Only `Type=MultiMissile`, `Type=LightningStorm` and `Type=PsychicDominator` weapons read it, and `Light.Enabled=no` turns it off.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Light.Ambient=100
```

See [Lighting](/systems/superweapons/#lighting).
