---
key: DeployingAnim
scope: unittype
label: 'Deploying animation'
see_also: [IsSimpleDeployer, DeployToLand]
when_omitted:
  kind: value
  value: none
---

The animation an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle plays in its place as it deploys. The vehicle is hidden until the animation's frames have all played at its `Rate`. Packing up plays no animation. Set the key in the vehicle's rules section.

```ini title="rulesmd.ini"
[MYCHOPPER] ; example VehicleType
IsSimpleDeployer=yes
DeployingAnim=MYCHOPPERDEPLOY
```
