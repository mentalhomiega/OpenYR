---
key: InfantryMutate
summary: The animation of an infantryman mutating into a brute; read but not used yet.
see_also: [InfDeath]
when_omitted:
  kind: value
  value: none
---

The key is read from `[AudioVisual]`, but mutation is not supported yet: a warhead with [`InfDeath=9`](/keys/infdeath/) makes the soldier play its explosion death sequence, and this animation does not play.

```ini title="rulesmd.ini"
[AudioVisual]
InfantryMutate=MYMUTATE ; an AnimType registered in [Animations]
```
