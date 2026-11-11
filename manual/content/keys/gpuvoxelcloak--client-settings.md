---
key: GpuVoxelCloak
scope: client-settings
label: GPU voxel cloak
when_omitted:
  kind: value
  value: Translucent
---

`GpuVoxelCloak=Ripple` makes [`GpuVoxels=yes`](/keys/gpuvoxels/) draw an object in the last stage of cloaking, as the game shows it to other players, with a distortion: the picture under the object is shifted by up to two map pixels by a wave that travels across it, and the object's translucent colour is added to the shifted picture. `Translucent`, the default, draws it as the plain translucent blend, as the software does. An unknown value reads as `Translucent`.

The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it. It changes only objects the game draws as a ripple, which are objects of other players in the last stage of cloaking or uncloaking. A cloaked object that its owner or an ally sees, and a warping object, stay plainly translucent. A pass with eight sonic waves has no room for ripples, and in a pass with fewer, each ripple takes the place of one more wave; an object beyond the room is drawn translucent.
