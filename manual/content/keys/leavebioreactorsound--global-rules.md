---
key: LeaveBioReactorSound
scope: global-rules
label: 'Bio Reactor leave sound'
see_also: [EnterBioReactorSound, InfantryAbsorb, UnitAbsorb]
when_omitted:
  kind: value
  value: none
---

The sound played at an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) or [`UnitAbsorb=yes`](/keys/unitabsorb/) structure each time an object it held comes back out onto the map. An object that finds no room and is removed plays no sound.
