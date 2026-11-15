---
key: Parachute.Anim
summary: The canopy animation drawn above a paradropped object of this type.
see_also: [Parachute, BombParachute, "system:superweapons"]
when_omitted:
  kind: inherited
  note: "[General] Parachute."
---

`Parachute.Anim` names the animation that hangs above an object of this type, soldier or vehicle, while it falls from a paradrop plane or a transport. It replaces [`Parachute`](/keys/parachute/) for the type.

Give the animation a [`LoopCount=`](/keys/loopcount/) long enough to play until landing, as for `Parachute`.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType
Parachute.Anim=PARACHBIG
```
