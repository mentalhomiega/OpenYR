---
key: DeployFire
scope: infantrytype
label: 'Fires while deployed'
see_also: [Deployer, DeployFireWeapon]
when_omitted:
  kind: value
  value: "no"
---

A [`Deployer=yes`](/keys/deployer/) soldier with `DeployFire=yes` keeps fighting while it is deployed. It then fires the weapon in the slot [`DeployFireWeapon`](/keys/deployfireweapon/#scope-infantrytype) names, and plays its `DeployedFire` [sequence](/keys/sequence/) for each shot. When it is not deployed, it fires only its first weapon, whatever the target.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeployFire=yes
Primary=MyRifle
Secondary=MyDugInRifle ; fired while deployed
```

Without `DeployFire`, a deployed soldier takes no targets, and an undeployed one chooses between its weapons as any other object does.
