---
key: IronCurtainInvokeAnim
summary: The animation played over the cell an Iron Curtain is fired at.
see_also: [IronCurtainDuration, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays at the center of the target cell, raised slightly above the ground, or above the bridge when the cell has one.

```ini title="rulesmd.ini"
[General]
IronCurtainInvokeAnim=MYCURTAIN ; an AnimType registered in [Animations]
```

With the key unset, the Iron Curtain still works but shows nothing where it lands.
