---
key: DeployFireWeapon
scope: infantrytype
label: 'Deployed weapon slot'
see_also: [DeployFire, Deployer]
when_omitted:
  kind: value
  value: "1"
---

A deployed [`DeployFire=yes`](/keys/deployfire/#scope-infantrytype) soldier fires the weapon in this slot: `0` for its primary weapon and `1` for its secondary.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeployFire=yes
DeployFireWeapon=0 ; keeps the primary weapon while deployed
```

The value has no effect on a soldier that does not set `DeployFire=yes`.
