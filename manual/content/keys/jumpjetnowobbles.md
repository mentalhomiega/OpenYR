---
key: JumpjetNoWobbles
summary: "Stops a jumpjet of this type bobbing while it flies."
see_also: [JumpjetDeviation, JumpjetWobbles]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a jumpjet of this type holds its flight height steady instead of bobbing by [`JumpjetDeviation`](/keys/jumpjetdeviation/).

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetNoWobbles=yes
```
