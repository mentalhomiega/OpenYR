---
title: Re-check buildability when the sidebar recalculates
category: feature
release: 0.2.0
targets:
- type: key
  id: RecheckPrerequisites
  effect: added
- type: system
  id: sidebar
  effect: changed
credit: [ZivDero]
---

`RecheckPrerequisites` in `[General]` of `rules.ini` sets whether the sidebar removes a cameo when the player stops meeting its tech level, prerequisites or, for a structure, ownership rules. The default is `yes`, which also cancels the item being built, every queued copy, and a structure waiting to be placed. With `no`, such a cameo stays and can still start production.
