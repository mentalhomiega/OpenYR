---
title: Build a type when the house owns an override structure
category: fix
release: 0.2.0
targets:
- type: key
  id: PrerequisiteOverride
  effect: added
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

`PrerequisiteOverride=` in a type's section now takes effect. A house that owns a structure on the list can build the type without its `Prerequisite=` entries, tech level, stolen-technology, country and structure `Owner=` checks, and only the build limit still applies. Before, the game did not read the key. The save revision moves to 65, so saves from earlier builds are not compatible.
