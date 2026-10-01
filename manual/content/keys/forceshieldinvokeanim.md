---
key: ForceShieldInvokeAnim
summary: "The animation played where the force shield is raised."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays over the force shield's target, raised slightly above the ground, or above the bridge when the cell has one.

```ini title="rulesmd.ini"
[General]
ForceShieldInvokeAnim=MYSHIELD ; an AnimType registered in [Animations]
```

With the key unset, the shield still works but shows nothing where it lands.
