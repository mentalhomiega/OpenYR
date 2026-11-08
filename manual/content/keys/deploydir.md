---
key: DeployDir
summary: "Facing a vehicle that lands to deploy turns to as it comes down."
see_also: [DeployToLand, IsSimpleDeployer, PoseDir]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rulesmd.ini"
[AudioVisual]
DeployDir=2
```

A vehicle with [`DeployToLand=yes`](/keys/deploytoland/), such as the Siege Chopper, turns to this facing while it descends to land. It does so on every landing, not only one that ends in deploying. The value is a compass step: `0` is north, `1` northeast, `2` east, and so on clockwise to `7`, northwest. A value above `7` wraps around the compass.
