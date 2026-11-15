---
title: Let a type refuse every cloak (Cloakable.Allowed)
category: feature
release: 0.2.0
targets:
- type: key
  id: Cloakable.Allowed
  effect: added
credit: [MentalHomiega]
---

An object whose type sets `Cloakable.Allowed=no` now never cloaks, by itself, in a Cloak Generator's field or through a crate, as Ares does; a cloak crate it picks up pays money. Types that leave the key out are unchanged.
