---
key: RadLevel
summary: "The radiation a weapon leaves where its projectile goes off."
see_also: [RadDurationMultiple, CellSpread, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

A value above `0` leaves a [radiation](/systems/radiation/) patch of this level, reaching the warhead's `CellSpread`. A shot into the center of an existing patch adds this to what the patch has left.

```ini title="rulesmd.ini"
[MyEruption] ; example Weapon
RadLevel=500
```
