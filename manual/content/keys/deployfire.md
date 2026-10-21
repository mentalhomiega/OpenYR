---
key: DeployFire
summary: "Lets a deployed soldier or simple deployer fire, and picks its weapon by whether it is deployed."
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

An [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle with `DeployFire=yes` fires the weapon in the slot [`DeployFireWeapon`](/keys/deployfireweapon/) names while it is deployed, and only its first weapon otherwise, whatever the target. Other vehicles ignore the key.

Aircraft and structures read the key and ignore it.
