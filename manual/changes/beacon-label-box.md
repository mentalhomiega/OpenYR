---
title: Draw the beacon message in a box below the marker
category: fix
release: 0.2.0
targets:
- type: command
  id: PlaceBeacon
  effect: changed
credit:
- MentalHomiega
---

A beacon's message now sits in a black box below the beacon marker, with a border in the owner's colour. The text uses the same font and colour as before, is left-aligned in the box, and wraps at 200 pixels. Before, the message was centred above the marker without a box.
