---
key: DeployFire
scope: unittype
label: 'Fires its deploy weapon while deployed'
see_also: [IsSimpleDeployer, DeployFireWeapon]
when_omitted:
  kind: value
  value: "no"
---

An [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle with `DeployFire=yes` fires the weapon in the slot [`DeployFireWeapon`](/keys/deployfireweapon/#scope-unittype) names while it is deployed, and only its first weapon otherwise, whatever the target. Other vehicles ignore the key.
