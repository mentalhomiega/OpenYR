---
key: Ripple.Radius
summary: "The older name of IonCannon.Ripple: the frames of the ion cannon's shockwave a warhead shows."
see_also: [IonCannon.Ripple]
when_omitted:
  kind: value
  value: "0, so the warhead shows no shockwave"
---

`Ripple.Radius` is the name older Ares rules use for [`IonCannon.Ripple`](/keys/ioncannon.ripple/), and it does the same: a warhead with a number above 0 shows the ion cannon's ground shockwave where it goes off, for that many frames of the wave's 79. A warhead that sets both keys takes `IonCannon.Ripple`.

```ini title="rulesmd.ini"
[SeismicWH]
Ripple.Radius=40
```
