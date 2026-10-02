---
key: DeploySound
scope: infantrytype
label: Soldier deploy sound
see_also: [Deployer, UndeploySound]
when_omitted:
  kind: value
  value: none
---

A [`Deployer=yes`](/keys/deployer/#scope-infantrytype) soldier plays this sound at its position when it starts to deploy.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeploySound=MyDigIn ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored and the sound set earlier stays.
