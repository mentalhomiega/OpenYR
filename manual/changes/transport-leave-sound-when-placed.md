---
title: Play the transport leave sound only when a passenger gets out
category: fix
release: 0.2.0
targets:
- type: key
  id: LeaveTransportSound
  effect: changed
credit:
- MentalHomiega
---

A transport plays its `LeaveTransportSound=` only when a passenger is placed outside it. Before, the sound also played when no room was found and the passenger stayed inside.
