---
title: Let a captured construction yard build what its type lists
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
- type: key
  id: MultiMCV
  effect: changed
credit: [MentalHomiega]
---

A construction yard builds a structure when the yard's type and the structure's `Owner` list share a country. The country the yard acts as no longer limits this. A captured Soviet construction yard therefore builds the Soviet structures whose prerequisites the house meets. `MultiMCV` is still read but changes nothing.
