---
key: ReversedAs
summary: Names the type that grinding up an object of this type teaches in place of its own.
see_also: [ReverseEngineersVictims, CanBeReversed, "system:production"]
when_omitted:
  kind: value
  value: "none; grinding teaches the object's own type"
---

The key is read on the victim's type. When a structure with [`ReverseEngineersVictims=yes`](/keys/reverseengineersvictims/) grinds up an infantryman or vehicle of this type, its owner learns the named type instead of the victim's own. The name is not case sensitive and is searched among the vehicle types, then the infantry types, then the aircraft types, and the first match is used.

A name that matches no such type teaches nothing, not the victim's own type. A structure type cannot be named. The setting does nothing on a type that sets [`CanBeReversed=no`](/keys/canbereversed/).

```ini title="rulesmd.ini"
[SNIPER] ; example InfantryType whose grinding teaches a different type
ReversedAs=SEAL
```
