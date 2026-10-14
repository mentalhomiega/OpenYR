---
title: Stop selecting objects that have not entered the playable area
category: fix
release: 0.2.0
targets:
- type: system
  id: band-selection
  effect: changed
credit:
- MentalHomiega
---

An object that has not entered the playable area can no longer be selected by a click or by a band box. Before, the selection test did not check this, so such an object could be selected. The object becomes selectable once it enters the playable area.
