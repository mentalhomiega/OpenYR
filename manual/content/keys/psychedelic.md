---
key: Psychedelic
summary: "Makes a warhead drive what it hits berzerk instead of damaging it."
see_also: [BerserkFriendly, ImmuneToPsionics, "system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

A `Psychedelic=yes` warhead [drives a vehicle, infantryman or aircraft berzerk](/systems/warheads/#what-the-target-loses) for as many frames as its damage after armor, instead of damaging it. Allies of the attacker, structures and `ImmuneToPsionics=yes` types are left alone.

```ini title="rulesmd.ini"
[MyMadnessGas] ; example Warhead
Psychedelic=yes
```
