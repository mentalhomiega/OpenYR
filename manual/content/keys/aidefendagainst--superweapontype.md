---
key: AIDefendAgainst
scope: superweapontype
label: Computer defense against a superweapon
summary: "Lets a computer house raise its force shield when this weapon is fired near its base."
see_also: ["system:superweapons", AISuperDefenseDistance, AISuperDefenseProbability, AISuperDefenseFrames]
when_omitted:
  kind: value
  value: "no"
---

With `AIDefendAgainst=yes`, every computer house reacts when a house fires this weapon, whoever fires it and whatever its target: when the target cell lies within [`AISuperDefenseDistance`](/keys/aisuperdefensedistance/) of the center of the house's base, the house rolls [`AISuperDefenseProbability`](/keys/aisuperdefenseprobability/) for its difficulty slot. On success it wants its force shield over its first construction yard, or over the center of its base when it has none, for the next [`AISuperDefenseFrames`](/keys/aisuperdefenseframes/) frames. [The computer's use of force shield](/systems/superweapons/#the-computers-force-shield) covers the rest.

```ini title="rulesmd.ini"
[NukeSpecial]
AIDefendAgainst=yes ; computer houses near the target raise their force shield
```

The weapon fires no differently when the key is `no`. A house that is human, passive or has no base ignores the weapon.
