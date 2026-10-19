---
key: Convert.Deploy
summary: The vehicle type a simple deployer of this type becomes when it finishes deploying.
see_also: [IsSimpleDeployer, DeployingAnim, DeployToLand]
when_omitted:
  kind: value
  value: "none"
---

`Convert.Deploy` names a vehicle type. When an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle of this type finishes deploying, after its [`DeployingAnim`](/keys/deployinganim/) has played, it becomes that type where it stands and is undeployed again, so another deploy order changes it once more. A new vehicle type that names the first one back makes a pair that a deploy order switches between.

The vehicle keeps its owner, rank, cell and the same share of its health, rounded down to at least 1. A vehicle that is above its new type's [`Ammo`](/keys/ammo/) keeps that many rounds. A type with another [`Locomotor`](/keys/locomotor/) is refused, and the vehicle deploys without converting. A vehicle that packs up converts nothing.

```ini title="rulesmd.ini"
[TRIKE] ; example VehicleType
IsSimpleDeployer=yes
Convert.Deploy=TRIKE2

[TRIKE2]
IsSimpleDeployer=yes
Convert.Deploy=TRIKE
```
