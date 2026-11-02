---
key: TurretNotExportedOnGround
summary: Read but ignored; a building's voxel barrel pivots about the turret origin whether this is set or not.
see_also: ["TurretOffset", "TurretAnimIsVoxel", "Voxel"]
when_omitted:
  kind: value
  value: "no"
---

OpenYR reads this key from the section of the object's Image ID and then does nothing with it. A building's voxel barrel pitches about the turret origin, after the [`TurretOffset`](/keys/turretoffset/) shift, whether the key is set or cleared. A vehicle is not affected either.
