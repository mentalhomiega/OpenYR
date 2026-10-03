---
key: DeploySound
scope: buildingtype
label: Structure undeploy sound
see_also: [AuxSound1, UndeploySound, UndeploysInto, BuildupSound]
when_omitted:
  kind: value
  value: none
  note: The structure plays its `AuxSound1` sound instead, or none when that key is unset too.
---

```ini title="rules.ini"
[MYSILO] ; a BuildingType registered in [BuildingTypes]
DeploySound=MYSILO_Packup ; a sound ID registered in SOUND.INI
```

A structure with [`UndeploysInto`](/keys/undeploysinto/) plays this sound at its position as it turns back into that vehicle, whichever house owns it. Its [`VoiceDeploy`](/keys/voicedeploy/) plays at the same moment for its owner. [`BuildupSound`](/keys/buildupsound/) sets the sound of its construction.

:::caution[It replaces `AuxSound1`]
`DeploySound` and [`AuxSound1`](/keys/auxsound1/) set the same sound on a BuildingType, and `DeploySound` is read second. A type that sets both keeps only `DeploySound`. A name that matches no registered sound is ignored, so a misspelled `DeploySound` leaves the `AuxSound1` sound in place.
:::
