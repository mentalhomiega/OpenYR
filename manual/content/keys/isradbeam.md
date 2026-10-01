---
key: IsRadBeam
summary: "Makes a weapon draw a wavy beam from its muzzle to its target as it fires."
see_also: [RadColor, ChronoBeamColor, Temporal]
when_omitted:
  kind: value
  value: "no"
---

Each shot draws a beam from the firing position to the target's center for 15 frames. The beam is a vertical wave, one cycle per 180 leptons of beam and two on a shorter beam, whose height grows to 40 leptons by its last frame; the wave's peaks are drawn brightest. The beam takes [`RadColor`](/keys/radcolor/), or [`ChronoBeamColor`](/keys/chronobeamcolor/) when the weapon's warhead is [`Temporal=yes`](/keys/temporal/). The beam is drawn only; the shot's projectile still carries the damage.

```ini title="rulesmd.ini"
[MyRadBeam] ; example Weapon
IsRadBeam=yes
```
