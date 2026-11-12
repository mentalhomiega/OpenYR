---
key: Missile.Custom
summary: "Makes an aircraft type fly as a launched missile with its own Missile.* settings."
see_also: ["Missile.Weapon", "Missile.Warhead", "Missile.PauseFrames", "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "no"
---

An Ares key. With `yes` the aircraft type flies as a missile launched from a spawner, using the `Missile.` keys of its own section for its pause, tilt, climb, cruise and detonation. A type with `no` that is named by `V3RocketType`, `DMislType` or `CMislType` in `[General]` still uses the `[General]` keys. On a type named there, `yes` replaces the `[General]` values with the type's own, which start at 0, so every `Missile.` key it needs must be set.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.Altitude=1500
Missile.Damage=300
Missile.Warhead=HE
```
