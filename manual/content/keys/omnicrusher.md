---
key: OmniCrusher
summary: "Lets a vehicle flatten every non-allied object that is not a building, whatever its Crushable flag says."
see_also: [Crusher, Crushable, OmniCrushResistant]
when_omitted:
  kind: value
  value: "no"
---

A vehicle with this flag and [`Crusher=yes`](/keys/crusher/) drives over any non-allied object that is not a building and is not [`OmniCrushResistant=yes`](/keys/omnicrushresistant/), even one with [`Crushable=no`](/keys/crushable/), such as a Tanya or a Chrono Legionnaire. Objects under the Iron Curtain are not crushed. The vehicle also moves onto such an object as its target when the target is out of weapon range, instead of standing off to fire at it.

```ini title="rulesmd.ini"
[MYFORTRESS] ; example VehicleType
Crusher=yes
OmniCrusher=yes
```
