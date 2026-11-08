---
key: DeployToLand
summary: "Makes a simple-deployer vehicle land before it deploys, and keeps a flying object's walk counter running."
see_also: [IsSimpleDeployer]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle that is told to deploy in the air first drops its target and lands on its cell, then deploys. Without it, the vehicle deploys at whatever height it is.

With `IsSimpleDeployer=yes` as well, a vehicle that is ordered to fly to a cell, rather than to deploy, hovers where it arrives instead of landing. It lands when it is told to deploy.

Once a flying vehicle has been told to deploy, it lands at the end of its next move too, and it hovers at a stop again only after it has touched down.

The key is read on every object type. An object with `DeployToLand=yes` that is above the ground advances its walk counter on every frame, not only on its walk-rate frames, unless it is warping or hit by a locomotor attack. Aircraft and vehicles draw their body animation from that counter.
