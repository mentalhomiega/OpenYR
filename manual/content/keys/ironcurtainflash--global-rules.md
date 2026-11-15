---
key: IronCurtainFlash
scope: global-rules
label: Flash on hits the Iron Curtain turns away
summary: "Whether a hit on an Iron Curtained or force-shielded object makes a flash over it."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

`IronCurtainFlash` is an Ares key in `[AudioVisual]`. A hit that an Iron Curtain or a Force Shield turns away makes a flash of light over the object, white under the Iron Curtain and blue under the Force Shield, as big as a blast of twice the damage. With `IronCurtainFlash=no` these hits make no flash. The protection and the damage are unchanged.

```ini title="rulesmd.ini"
[AudioVisual]
IronCurtainFlash=no
```
