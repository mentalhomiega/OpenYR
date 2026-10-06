---
key: AttachEffect.FirepowerMultiplier
scope: warheadtype
label: Firepower multiplier
when_omitted:
  kind: value
  value: "1.0"
  note: "Damage dealt is unchanged."
---

`AttachEffect.FirepowerMultiplier` multiplies the damage of each shot fired by an object carrying this warhead's effect, together with the owner's country firepower bonus, crate firepower and veteran firepower, rounded down. The damage is set when the shot is fired, so a projectile already in flight when the effect starts or ends keeps the damage it had. Weapons with `IsSonic=yes` or `UseFireParticles=yes` are not changed.

```ini title="rulesmd.ini"
[Frenzy] ; example Warhead
AttachEffect.Duration=300
AttachEffect.FirepowerMultiplier=2 ; a Grizzly's 65-damage shell does 130
```
