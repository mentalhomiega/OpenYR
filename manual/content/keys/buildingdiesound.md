---
key: BuildingDieSound
summary: The sound played where a structure is destroyed, unless its type has a DieSound.
see_also: [DieSound, CrumbleSound, BuildingDamageSound]
when_omitted:
  kind: value
  value: none
---

When damage destroys a structure whose type lists no [`DieSound`](/keys/diesound/), this sound plays at the structure's position. A structure with a `DieSound` list plays one entry of that list instead. Selling a structure does not play it.

[`CrumbleSound`](/keys/crumblesound/) is the older name of this key. A `BuildingDieSound` entry in the same section replaces a `CrumbleSound` entry.

```ini title="rulesmd.ini"
[AudioVisual]
BuildingDieSound=BuildingGenericDie ; a sound ID registered in SOUND.INI
```
