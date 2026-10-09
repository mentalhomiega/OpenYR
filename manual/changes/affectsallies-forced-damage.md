---
title: Apply AffectsAllies=no to forced damage
category: fix
release: 0.2.0
targets:
- type: key
  id: AffectsAllies
  effect: changed
credit:
- MentalHomiega
---

Before, forced damage such as a C4 charge passed through an `AffectsAllies=no` warhead to an object owned by an ally of the attacker. We now stop it, so a forced hit from an attacker in an allied house does nothing, as an unforced hit does.
