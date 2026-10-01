---
key: PsychicRevealRadius
summary: "How many cells the psychic reveal uncovers around its target."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "3"
---

The psychic reveal uncovers every cell within this many cells of its target for the firing house. [Psychic reveal](/systems/superweapons/#psychic-reveal) covers the weapon.

```ini title="rulesmd.ini"
[CombatDamage]
PsychicRevealRadius=15
```

Distance is measured in a straight line between cell centers.
