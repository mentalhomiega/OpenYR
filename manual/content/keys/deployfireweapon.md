---
key: DeployFireWeapon
summary: "The weapon slot a deployed soldier or simple deployer fires from."
see_also: [DeployFire, Deployer, IsSimpleDeployer]
when_omitted:
  kind: value
  value: "1"
---

A deployed [`DeployFire=yes`](/keys/deployfire/) soldier or simple deployer fires the weapon in this slot: `0` for its primary weapon and `1` for its secondary.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeployFire=yes
DeployFireWeapon=0 ; keeps the primary weapon while deployed
```

The value has no effect on an object that does not set `DeployFire=yes`. Aircraft and structures ignore it.
