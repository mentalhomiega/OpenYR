---
key: Inaccurate
summary: Scatters the aim point of an arcing shot before its arc is solved.
see_also: [Arcing, BallisticScatter]
when_omitted:
  kind: value
  value: "no"
---

An inaccurate [`Arcing=yes`](/keys/arcing/) projectile is aimed at a point scattered away from its target. Without `Arcing`, the setting does not move the aim point; its smaller effects are described below.

The scatter distance is drawn between half of [`[CombatDamage] BallisticScatter`](/keys/ballisticscatter/) and all of it, in a random direction. At the engine default that is half a cell to a whole cell. The distance does not grow with range: a shot at maximum range is scattered no more than a point-blank one. The exception is a flak shell with [`FlakScatter=yes`](/keys/flakscatter/), which scatters by its distance to the target. The firer solves its arc for the scattered point, so the shell comes down short, long or wide of the target. If the arc cannot reach the scattered point, that shot is not fired.

An inaccurate shell is never moved onto its target, so its scatter shows in where it bursts. It still bursts as it enters its target's cell less than 208 leptons above the ground, wherever in that cell it is, and the warhead's spread decides whether the target takes damage. [Where the blast lands](/systems/projectile-flight/#where-the-blast-lands) lists the moves an accurate shell gets.

```ini title="rules.ini"
[MYARTILLERYSHELL] ; a BulletType, registered by a weapon naming it as its Projectile
Image=120MM
Arcing=yes
Inaccurate=yes
```

Two smaller effects apply with or without `Arcing`:

- **The moves onto a target are skipped.** A projectile that goes off within 32 leptons of its target's center is normally moved onto it. So is one that goes off within 42 leptons of a target on the ground, or within 128 leptons of an airborne one, unless its warhead is [`EMEffect=yes`](/keys/emeffect/) or it is [`Splits=yes`](/keys/splits/). An inaccurate projectile is moved onto none of them, so its blast stays where it goes off.
- **A computer player may keep the firer in the middle of its base.** When a computer player sends a vehicle, infantry soldier or aircraft to wait in its base, such as one it has just produced, it sends it either to the middle of the base or near an edge. It chooses the middle only when that object has no anti-air, anti-armor or anti-infantry value. An object whose primary weapon fires this projectile has half the anti-armor and anti-infantry value, so a weak weapon can round down to zero.

Firing on the move does not make a shot inaccurate. Only this setting does.
