---
title: Release the passengers of a destroyed abductor
category: feature
release: 0.2.0
targets:
- type: key
  id: Abductor
  effect: changed
- type: system
  id: transports
  effect: changed
credit: [MentalHomiega]
---

The passengers of a destroyed vehicle that has an Abductor weapon get out even when forced damage, a fall or `Crashable=yes` would have killed them, onto a neighbouring cell when the vehicle's own cell is closed to them.
