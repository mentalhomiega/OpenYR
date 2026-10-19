---
key: SW.Range
scope: superweapontype
summary: "Sets the area an iron curtain, psychic reveal or sonar pulse covers."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "3,3 for an iron curtain, PsychicRevealRadius for a psychic reveal, 10 for a sonar pulse"
---

One number is a radius in cells. Two whole numbers are the width and height of a block of cells centered on the target, so `3,3` is the target cell and the eight around it. A negative radius covers the whole map. A `Type=PsychicReveal` reads only a radius. A `Type=PsychicReveal` or `Type=SonarPulse` weapon with a negative radius fires from the cameo, with no aiming. Other behaviors ignore the key.

[`Range=`](/keys/range/#scope-superweapontype) only draws the ring around the cursor.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Range=5
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
