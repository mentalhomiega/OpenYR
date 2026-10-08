---
key: DeployToLand
scope: unittype
label: 'Lands before deploying'
see_also: [IsSimpleDeployer]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle that is told to deploy in the air first drops its target and lands on its cell, then deploys. Without it, the vehicle deploys at whatever height it is.

With `IsSimpleDeployer=yes` as well, a vehicle that is ordered to fly to a cell, rather than to deploy, hovers where it arrives instead of landing. It lands when it is told to deploy.
