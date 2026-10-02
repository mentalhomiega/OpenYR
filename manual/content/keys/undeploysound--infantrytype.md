---
key: UndeploySound
scope: infantrytype
label: Soldier pack-up sound
see_also: [Deployer, DeploySound]
when_omitted:
  kind: value
  value: none
---

A [`Deployer=yes`](/keys/deployer/#scope-infantrytype) soldier plays this sound at its position when it starts to pack up from a deployment.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
UndeploySound=MyPackUp ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored and the sound set earlier stays.
