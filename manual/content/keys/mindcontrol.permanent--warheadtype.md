---
key: MindControl.Permanent
scope: warheadtype
summary: "With yes, a mind control warhead takes its target for good, as the psychic dominator does."
see_also: [MindControl, PermaControlledAnimationType, "system:mind-control"]
when_omitted:
  kind: value
  value: "no"
---

`MindControl.Permanent` is an Ares key that needs [`MindControl=yes`](/keys/mindcontrol/) on the same warhead. With `yes`, a hit gives the target to the firer's house for good, as the psychic dominator does, and the target shows [`PermaControlledAnimationType`](/keys/permacontrolledanimationtype/) above it. The firer keeps no link to what it took: there is no limit on how many objects it takes, nothing goes back when the firer dies, and the weapon need not be the firer's primary weapon.

A permanent hit also takes an object that another controller holds, conventionally or for good; a conventional controller first lets it go. It takes structures too, wherever the weapon can fire at them. It does not take an object of the firer's own house, an `ImmuneToPsionics` type, a vehicle in a bunker, anything under the Iron Curtain, a structure being built or sold, or a structure a trigger handed to another house. A unit a computer house takes this way goes hunting.

```ini title="rulesmd.ini"
[MyDominatorBeamWH] ; example Warhead
MindControl=yes
MindControl.Permanent=yes
```
