---
key: AmbientSound
scope: animtype
label: Ambient sound of an animation
see_also: [CrushSound]
when_omitted:
  kind: value
  value: none
---

`AmbientSound=` in an animation's art section names a sound that plays at each animation of the type while it is on the map. The sound follows the rules of the [object-type key](/keys/ambientsound/): it is heard from the animation's position, a looping sound runs for as long as the animation lasts, and a sound that is not a loop starts again each time it ends. A name that matches no sound leaves the sound an earlier file set.
