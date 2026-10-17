---
title: Leave an absorbed infantryman out of its owner's infantry count
category: fix
release: 0.2.0
targets:
- type: key
  id: InfantryAbsorb
  effect: changed
credit:
- MentalHomiega
---

An infantryman inside an [`InfantryAbsorb=yes`](/keys/infantryabsorb/) structure no longer counts as one of its owner's infantry until it comes out. Before, it still counted, so a map event that waited for the owner to lose all its infantry did not fire while one sat inside a live Bio Reactor. Selling the structure or having it destroyed works as before.
