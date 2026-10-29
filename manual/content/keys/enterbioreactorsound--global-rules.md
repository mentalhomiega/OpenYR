---
key: EnterBioReactorSound
scope: global-rules
label: 'Bio Reactor enter sound'
see_also: [LeaveBioReactorSound, InfantryAbsorb, UnitAbsorb]
when_omitted:
  kind: value
  value: none
---

The sound played each time an infantryman goes into an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) structure, such as the Bio Reactor. It plays at the infantryman's position, before it goes inside. A vehicle that goes into a [`UnitAbsorb=yes`](/keys/unitabsorb/) structure plays no sound.
