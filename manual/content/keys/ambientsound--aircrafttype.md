---
key: AmbientSound
scope: aircrafttype
label: Ambient sound of an object type
see_also: [CrushSound, MoveSound]
when_omitted:
  kind: value
  value: none
---

`AmbientSound=` names a sound from the sound list that plays at each object of the type while the object is on the map. The sound is heard from the object's position, so it fades with distance and is silent when the object is off the visible view. An object that is inside a transport or otherwise out of play is silent until it returns. The sound stops when the object is destroyed or removed.

A looping sound runs for as long as the object is in earshot. A sound that is not a loop starts again each time it ends. The engine keeps the sounds of at most 256 objects at once, and an object that finds the table full has none.

```ini title="rules.ini"
[MYFOUNTAIN] ; a TerrainType
AmbientSound=FountainLoop ; a looping sound defined in the sound list
```

A name that matches no sound, or an empty value, leaves the sound an earlier file set. The key works on every kind of object that takes part in the game's per-frame update: vehicles, infantry, aircraft, structures, terrain and projectiles. An animation names its sound in the art file; see [`AmbientSound` on an AnimType](/keys/ambientsound/#scope-animtype).
