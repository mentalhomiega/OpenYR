---
key: LeaveBioReactorSound
scope: global-rules
label: 'Bio Reactor leave sound'
see_also: [EnterBioReactorSound, InfantryAbsorb, UnitAbsorb]
when_omitted:
  kind: value
  value: none
---

The sound played at an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) structure each time an object it held comes back out onto the map. The sound plays at the structure's centre. A structure that holds objects only through [`UnitAbsorb=yes`](/keys/unitabsorb/) plays none. An object that finds no room and is removed plays no sound.
