---
key: IsSimpleDeployer
scope: unittype
label: 'Deploys where it stands'
see_also: [DeployToLand, DeployingAnim, DeployFire, DeployFireWeapon, UnloadingClass]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the deploy command switches the vehicle between its normal and deployed states where it stands, without turning it into a structure. The Siege Chopper works this way: it lands and becomes artillery.

```ini title="rulesmd.ini"
[MYCHOPPER] ; example VehicleType
IsSimpleDeployer=yes
DeployToLand=yes
DeployingAnim=MYCHOPPERDEPLOY
UnloadingClass=MYCHOPPERDEPLOYED
DeployFire=yes
Secondary=MyArtilleryGun ; fired while deployed
```

The vehicle deploys in these steps:

1. A [`DeployToLand=yes`](/keys/deploytoland/#scope-unittype) vehicle in the air drops its target and lands on its cell.
2. The [`DeployingAnim`](/keys/deployinganim/#scope-unittype) plays in its place, and the vehicle is hidden until it ends.
3. The vehicle becomes deployed and goes on guard.

While deployed, the vehicle is drawn with the artwork of its `UnloadingClass` when that names a VehicleType. It cannot be given movement orders and does not move toward targets, so it attacks only what its weapons reach. A [`DeployFire=yes`](/keys/deployfire/#scope-unittype) vehicle fires its [`DeployFireWeapon`](/keys/deployfireweapon/#scope-unittype) only while deployed. Packed up, it picks from its two weapons as any vehicle does. Deploying again plays the `DeployingAnim` backward in the vehicle's place and packs it up. A `DeployToLand=yes` vehicle then moves to a nearby cell, so it takes off; any other vehicle stays where it is. It can then move and fly as before.
