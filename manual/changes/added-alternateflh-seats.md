---
title: Give every passenger seat of a large transport its own offset
category: feature
release: 0.2.0
targets:
- type: system
  id: transports
  effect: changed
credit: [MentalHomiega]
---

A transport with more than five `Passengers` reads one `AlternateFLH` key for each seat, up to 255 seats.
