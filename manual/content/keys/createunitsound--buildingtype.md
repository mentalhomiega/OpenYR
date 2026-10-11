---
key: CreateUnitSound
scope: buildingtype
label: Structure create sound
see_also: [CreateSound]
when_omitted:
  kind: value
  value: none
---

The sound played each time a house finishes producing an object that this structure can build, whatever the object is. It plays in addition to the object's own create sound.

For the player's house it plays where the object appears, once the object has left the factory or been placed. The structure used is the one the game then finds able to build the object. A computer house plays it at the structure that finished the object.

```ini title="rulesmd.ini"
[GAWEAP] ; a BuildingType that produces vehicles
CreateUnitSound=MyBuiltSound
```

The stock Yuri Grinder and Yuri Cloning Vats set this key, but neither builds anything, so it never plays for them.
