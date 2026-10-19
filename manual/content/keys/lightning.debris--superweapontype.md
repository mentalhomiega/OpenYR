---
key: Lightning.Debris
scope: superweapontype
summary: "Lists the debris animations a lightning bolt can throw up."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[AudioVisual] MetallicDebris"
---

A bolt that throws up debris, as [`Lightning.DebrisMin=`](/keys/lightning.debrismin/) describes, picks each piece from this list at random. An empty list, written `none`, throws up none.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Lightning.Debris=none
```

See [Lightning storm](/systems/superweapons/#lightning-storm).
