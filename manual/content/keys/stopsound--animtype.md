---
key: StopSound
scope: animtype
label: Animation end sound
see_also: ["ExpireSound", "BounceSound", "StartSound"]
when_omitted:
  kind: value
  value: none
---

The sound plays once at the animation's position when the game deletes the animation. That includes an animation that ends on its own and one removed early, such as one attached to an object that is sold or destroyed.

An animation that a trigger action creates as inert plays nothing. Voxel animations do not read this key.

An unrecognized sound name counts as omitted.
