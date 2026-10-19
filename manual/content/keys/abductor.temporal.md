---
key: Abductor.Temporal
summary: "Makes an Abductor weapon with a Temporal warhead take its target aboard when the warp finishes."
see_also: [Abductor, Temporal, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "no"
---

With `Abductor.Temporal=yes` on an [`Abductor=yes`](/keys/abductor/) weapon whose warhead sets [`Temporal=yes`](/keys/temporal/), the hit only starts the warp. When the warp has run out, the firer takes the target aboard in place of erasing it, on the same terms as any abduction. A target the firer cannot take, such as a structure, a vehicle that does not fit, or one above [`Abductor.MaxHealth`](/keys/abductor.maxhealth/), is erased as usual.

Without this key, an abductor weapon with a temporal warhead takes the target on the hit, and the warp never starts.

```ini title="rulesmd.ini"
[MyChronoPrison] ; example WeaponType
Abductor=yes
Abductor.Temporal=yes
Warhead=MyTemporalWH
```
