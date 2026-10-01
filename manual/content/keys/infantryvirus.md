---
key: InfantryVirus
summary: The animation left where a virus warhead kills an infantryman.
see_also: [InfDeath]
when_omitted:
  kind: value
  value: none
---

When a warhead with [`InfDeath=8`](/keys/infdeath/) kills an infantryman, this animation plays at his position and the soldier is removed at once, leaving no corpse.

```ini title="rulesmd.ini"
[AudioVisual]
InfantryVirus=MYVIRUS ; an AnimType registered in [Animations]
```

With the key unset, the soldier is removed without an animation.
