---
key: IronCurtain.Modifier
summary: "Multiplies the length of an Iron Curtain put on an object of this type; 0 makes it immune."
see_also: [IronCurtainDuration, "system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

A positive Iron Curtain duration is multiplied by this value and cut to a whole number of frames when the curtain is put on the object. At `0` the object is immune: it is not protected, and an infantryman or [`Organic=yes`](/keys/organic/) vehicle that the curtain would kill survives. The Force Shield is not changed by the key. Write the value as a number such as `0.8` or as a percentage.

```ini title="rulesmd.ini"
[MYHERO] ; example InfantryType
IronCurtain.Modifier=0
```
