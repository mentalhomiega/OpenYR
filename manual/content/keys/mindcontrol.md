---
key: MindControl
summary: "Makes a warhead take its target over for the firer instead of damaging it."
see_also: [InfiniteMindControl, ImmuneToPsionics, "system:mind-control"]
when_omitted:
  kind: value
  value: "no"
---

A weapon with this warhead takes over what it hits and deals no damage. It works only as the primary weapon an object has when it is placed on the map. [Mind control](/systems/mind-control/) covers what the weapon can take, how many objects its firer holds, and when they go back.

```ini title="rulesmd.ini"
[Controller] ; example Warhead
MindControl=yes
```
