---
key: StartSound
scope: animtype
label: Animation creation sound
see_also: ["Report", "BounceSound", "ExpireSound"]
when_omitted:
  kind: value
  value: none
---

`StartSound=` names the sound the animation plays at its position when it starts. When it names a registered sound, that sound replaces [`Report`](/keys/report/#scope-animtype) as the animation's start sound. `Report` is used only when `StartSound` is absent or names no registered sound.

The sound plays again each time the animation starts over, at the points [`Report`](/keys/report/#scope-animtype) lists.

On a voxel animation, [`StartSound`](/keys/startsound/#scope-voxelanimtype) does play when the animation is created.
