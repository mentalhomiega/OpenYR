---
key: SW.Range
scope: superweapontype
summary: "Sets the area an iron curtain, chronosphere, genetic mutator, psychic dominator, psychic reveal, sonar pulse or lightning storm covers."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "3,3 for an iron curtain, chronosphere or genetic mutator, PsychicRevealRadius for a psychic reveal, DominatorCaptureRange for a psychic dominator, 10 for a sonar pulse, LightningCellSpread for a lightning storm"
---

One number is a radius in cells. Two whole numbers are the width and height of a block of cells centered on the target, so `3,3` is the target cell and the eight around it. A negative radius covers the whole map. A `Type=ChronoSphere` weapon picks the units in the area, and a `Type=GeneticConverter` weapon without `MutateExplosion=yes` mutates the infantry in it. A `Type=PsychicDominator` weapon takes units over within the radius, or the block, of the target. For a `Type=LightningStorm` weapon, one number is the diameter of the area the scattered clouds fall in, not the radius. A `Type=PsychicReveal` reads only a radius. A `Type=PsychicReveal` or `Type=SonarPulse` weapon with a negative radius fires from the cameo, with no aiming. A `Type=ForceShield` weapon reads a radius in cells in place of `ForceShieldRadius`. Other behaviors ignore the key.

[`Range=`](/keys/range/#scope-superweapontype) only draws the ring around the cursor.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Range=5
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
