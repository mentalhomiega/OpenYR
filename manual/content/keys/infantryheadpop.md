---
key: InfantryHeadPop
summary: The animation left where a head-popping warhead kills an infantryman.
see_also: [InfDeath]
when_omitted:
  kind: value
  value: none
---

When a warhead with [`InfDeath=6`](/keys/infdeath/) kills an infantryman, this animation plays at his position and the soldier is removed at once, leaving no corpse.

```ini title="rulesmd.ini"
[AudioVisual]
InfantryHeadPop=MYHEADPOP ; an AnimType registered in [Animations]
```

With the key unset, the soldier is removed without an animation.
