---
key: DeploySound
scope: unittype
label: Vehicle deploy sound
see_also: [UndeploySound, DeploysInto, IsSimpleDeployer, AuxSound1]
when_omitted:
  kind: value
  value: none
  note: The vehicle plays its `AuxSound1` sound instead, or none when that key is unset too.
---

A vehicle plays this sound at its position as it deploys, whichever house owns it. A vehicle with [`DeploysInto`](/keys/deploysinto/) plays it as it unpacks into its structure, and an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/) vehicle as it starts to deploy.

```ini title="rulesmd.ini"
[SMIN] ; Slave Miner
DeploySound=SlaveMinerDeploy
```
