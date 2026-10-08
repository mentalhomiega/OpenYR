---
title: Stop enemies from targeting spies that look like their soldiers
category: fix
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
credit: [MentalHomiega]
---

A disguised soldier is no longer picked as a target, or fired back at, by a house that it looks like a friend to. Before, the scan skipped only infantry whose type sets `Disguised=yes`, so a spy in disguise was attacked by every enemy. A `DetectDisguise` structure now shows a disguised soldier in its covered cells to its owner's scans.
