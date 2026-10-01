---
key: InfantryBrute
summary: The animation left where a brute-killing warhead kills an infantryman.
see_also: [InfDeath]
when_omitted:
  kind: value
  value: none
---

When a warhead with [`InfDeath=10`](/keys/infdeath/) kills an infantryman, this animation plays at his position and the soldier is removed at once, leaving no corpse.

```ini title="rulesmd.ini"
[General]
InfantryBrute=MYBRUTEDIE ; an AnimType registered in [Animations]
```

The key is read from `[General]`, where Yuri's Revenge keeps it, and also from `[AudioVisual]`; an `[AudioVisual]` entry wins.

With the key unset, the soldier is removed without an animation.
