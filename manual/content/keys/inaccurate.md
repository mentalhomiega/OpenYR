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

A shell that is not [`Bouncy=yes`](/keys/bouncy/) still bursts on its target when it comes down within a cell and a half of it, or farther for a fast shell, unless it is [`Airburst=yes`](/keys/airburst/). At the engine default the scatter stays inside that reach, so for such a shell the scatter shows in its flight but not in where it bursts. [Where the blast lands](/systems/projectile-flight/#where-the-blast-lands) gives the exact reach.

A `Bouncy=yes` shell, such as the Firestorm Juggernaut's, bursts where it comes to rest, so its scatter does show in where it bursts. The exception is a shell that comes down in a cell holding an object that is not friendly; that shell is moved onto its target within the same reach.

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
