---
key: ForceShield.Modifier
summary: "Multiplies the length of a Force Shield put on an object of this type; 0 or less makes it immune."
see_also: [IronCurtain.Modifier, ForceShieldDuration, "system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

A positive Force Shield duration is multiplied by this value and cut to a whole number of frames when the shield is put on the object. At `0` or below the object is not protected at all. The Iron Curtain is not changed by the key; [`IronCurtain.Modifier`](/keys/ironcurtain.modifier/) scales that. Write the value as a number such as `0.5` or as a percentage.

```ini title="rulesmd.ini"
[MYPLANT] ; example BuildingType
ForceShield.Modifier=0
```
