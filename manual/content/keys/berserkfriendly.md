---
key: BerserkFriendly
summary: "Keeps berzerk objects from firing at objects of this type."
see_also: [Psychedelic, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

A [berzerk object](/systems/target-selection/#berzerk-objects) does not fire at an object of a `BerserkFriendly=yes` type, so a unit that spreads madness is not attacked by its own victims.

```ini title="rulesmd.ini"
[MYGASDRONE] ; example VehicleType
BerserkFriendly=yes
```
