---
key: DeployFire
summary: "Lets a deployed soldier or simple deployer fire, and lets a vehicle without passenger seats fire at its own cell."
see_also: [Deployer, IsSimpleDeployer, DeployFireWeapon]
when_omitted:
  kind: value
  value: "no"
---

A [`Deployer=yes`](/keys/deployer/#scope-infantrytype) soldier with `DeployFire=yes` keeps fighting while it is deployed. It then fires the weapon in the slot [`DeployFireWeapon`](/keys/deployfireweapon/) names, and plays its `DeployedFire` [sequence](/keys/sequence/) for each shot. When it is not deployed, it fires only its first weapon, whatever the target.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeployFire=yes
Primary=MyRifle
Secondary=MyDugInRifle ; fired while deployed
```

Without `DeployFire`, a deployed soldier takes no targets, and an undeployed one chooses between its weapons as any other object does.

An [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle with `DeployFire=yes` fires the weapon in the slot [`DeployFireWeapon`](/keys/deployfireweapon/) names while it is deployed, and only its first weapon otherwise, whatever the target.

A vehicle without `IsSimpleDeployer` and without passenger seats fires its first weapon at its own cell when the player clicks it while it is the only selected object. It keeps that order until it gets another. Only an [`AreaFire=yes`](/keys/areafire/#scope-weapontype) weapon reaches the vehicle's own cell, so a first weapon without that flag does not fire. Such a vehicle never deploys and never fires its `DeployFireWeapon`. A vehicle with passenger seats, or one in a bunker, does not fire at its own cell.

Aircraft and structures read the key and ignore it.
