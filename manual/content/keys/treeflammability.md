---
key: TreeFlammability
summary: The chance a burning terrain object would set fire to a neighbor. No terrain object catches fire in gamemd, so this has no effect.
no_effect: true
see_also: [TreeFire, IsFlammable]
when_omitted:
  kind: value
  value: ".1"
---

gamemd never tests this chance. Terrain objects do not catch fire, so no fire spreads from a tree to the terrain objects beside it.
