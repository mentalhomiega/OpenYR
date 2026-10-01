---
key: InfantryMutate
summary: The animation of an infantryman mutating, which becomes a new infantryman.
see_also: [InfDeath, MakeInfantry, AnimToInfantry, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

When a warhead with [`InfDeath=9`](/keys/infdeath/) kills an infantryman, he is removed and this animation plays where he stood. It belongs to the house of the object that dealt the damage, or of the superweapon that did. When it ends, its [`MakeInfantry`](/keys/makeinfantry/) entry turns it into a new infantryman of that house, such as a brute.

```ini title="rulesmd.ini"
[General]
InfantryMutate=MYMUTATE ; an AnimType registered in [Animations]
```

The key is read from `[General]`, where Yuri's Revenge keeps it, and also from `[AudioVisual]`; an `[AudioVisual]` entry wins. With the key unset, the soldier is removed without an animation.
