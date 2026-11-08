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

When a building swaps one of its attached animations for another, such as its damaged form, the old animation is removed without this sound, as in Yuri's Revenge.

An animation that a trigger action creates as inert plays nothing. Voxel animations do not read this key.

An unrecognized sound name counts as omitted.
