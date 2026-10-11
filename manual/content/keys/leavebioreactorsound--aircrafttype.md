---
key: LeaveBioReactorSound
scope: aircrafttype
label: Per-type Bio Reactor leave sound
see_also: [EnterBioReactorSound, InfantryAbsorb, UnitAbsorb]
when_omitted:
  kind: value
  value: none
---

Accepted in an object type's entry, but nothing plays it. An absorbing structure plays the [`LeaveBioReactorSound`](/keys/leavebioreactorsound/#scope-global-rules) in `[AudioVisual]` for every object it releases, whatever sound the object's type sets.
