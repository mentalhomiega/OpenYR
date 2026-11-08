---
key: DeployingAnim
scope: unittype
label: 'Deploying animation'
see_also: [IsSimpleDeployer, DeployToLand]
when_omitted:
  kind: value
  value: none
---

The animation an [`IsSimpleDeployer=yes`](/keys/issimpledeployer/#scope-unittype) vehicle plays in its place as it deploys, and plays backward as it packs up. The vehicle is hidden while the animation plays. Deploying shows the vehicle again as the animation starts its last frame, after `End` - 1 waits of `Rate`. Packing up shows it again after `End` - 2 waits, one wait before the animation reaches its first frame. The frames are drawn in the owner's unit colors. A vehicle with [`Palette=`](/keys/palette/) draws them through that palette instead, still in the owner's colors. Set the key in the vehicle's rules section.

```ini title="rulesmd.ini"
[MYCHOPPER] ; example VehicleType
IsSimpleDeployer=yes
DeployingAnim=MYCHOPPERDEPLOY
```
