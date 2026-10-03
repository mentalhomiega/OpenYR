---
key: UndeploySound
scope: unittype
label: Vehicle pack-up sound
see_also: [DeploySound, IsSimpleDeployer, AuxSound2]
when_omitted:
  kind: value
  value: none
  note: The vehicle plays its `AuxSound2` sound instead, or none when that key is unset too.
---

An [`IsSimpleDeployer=yes`](/keys/issimpledeployer/) vehicle plays this sound at its position as it starts to pack up. A vehicle that deploys into a structure ignores it; the structure's [`DeploySound`](/keys/deploysound/#scope-buildingtype) plays when it turns back.

```ini title="rulesmd.ini"
[MYSIEGE] ; example VehicleType
UndeploySound=MYSIEGE_Packup
```
