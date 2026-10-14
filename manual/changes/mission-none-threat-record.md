---
title: Read the default mission record for an object with no mission
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
breaking: false
credit:
- MentalHomiega
---

An object with no mission took its threat flag from memory outside the mission table. A target scan reads that flag, so the same object could be a target on one machine of a network game and not on the other. Such an object now takes the default mission record, where the flag is off, so a defensive structure can choose it as a target.

In the traced test runs the old read gave a nonzero flag every time, so those objects were not targets. The fix changes that for every object with no mission.
