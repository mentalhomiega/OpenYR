---
key: Deployer
summary: Lets the player dig this soldier in, as the GI deploys behind sandbags.
see_also: [DeployFire, DeployFireWeapon, DeploySound, UndeploySound, Sequence]
when_omitted:
  kind: value
  value: "no"
---

A `Deployer=yes` soldier deploys when the player gives it the Deploy command or clicks the soldier while it is the only object selected. It plays its `Deploy` [sequence](/keys/sequence/) and then holds its `Deployed` frames. The same order packs it up again: it plays `Undeploy` and stands ready.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Deployer=yes
DeployFire=yes      ; fires while dug in
DeployFireWeapon=1  ; with its secondary weapon
```

While a human player's soldier is deployed:

- it ignores move orders, and the cursor shows that it cannot move;
- it never lies down and plays no idle animations;
- it fires only if its type sets [`DeployFire=yes`](/keys/deployfire/), and then only at targets within the reach of its [`DeployFireWeapon`](/keys/deployfireweapon/). Without `DeployFire`, it takes no targets at all.

A computer player's soldier that is told to move packs up first and then walks off. A soldier that starts walking any other way while deployed leaves its deployment without playing `Undeploy`.

The soldier plays [`DeploySound`](/keys/deploysound/#scope-infantrytype) as it deploys and [`UndeploySound`](/keys/undeploysound/#scope-infantrytype) as it packs up. A sequence missing from the artwork has no frames, so a type without `Deploy` frames cannot deploy at all.
