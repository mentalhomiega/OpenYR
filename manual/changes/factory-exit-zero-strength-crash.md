---
title: Stop a unit that left a factory with no radio contact from crashing
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit: [MentalHomiega]
---

A vehicle that left a war factory with zero strength no longer crashes the game. Its radio link to the factory was refused, but the factory still tethered it, and the unit's next target scan read the missing contact. The tether is kept, as before, and the contact check now skips a unit with no contact.
