---
key: IonCannon.Ripple
summary: "The frames of the ion cannon's shockwave that this warhead shows where it goes off."
see_also: [Ripple.Radius, Dominator.Ripple]
when_omitted:
  kind: value
  value: "0, so the warhead shows no shockwave"
---

`IonCannon.Ripple` is an Ares key. A warhead with a number above 0 shows the ion cannon's ground shockwave at the place it goes off, for that many frames of the wave's 79; a larger number lets the ring travel further. The effect is visual only: it damages nothing and moves nothing. A number above 79 counts as 79.

A place that got a ripple in the last 15 frames gets no second one, so a weapon that hits the same cell every few frames shows one wave at a time.

Older Ares rules call the key [`Ripple.Radius`](/keys/ripple.radius/). We read both, and `IonCannon.Ripple` wins when a warhead sets both.

Ares describes the number as the frames of effect strength. We run the wave from its start for that many frames, which is our reading of it; a comparison with Ares itself has not been made.

```ini title="rulesmd.ini"
[IonCannonWH]
IonCannon.Ripple=79
```
