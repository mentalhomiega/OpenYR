---
title: Number the firing animation rounds from FiringSyncFrame0
category: fix
release: 0.2.0
targets:
- type: key
  id: FiringSyncFrame0
  effect: added
- type: key
  id: FiringSyncFrame1
  effect: changed
- type: key
  id: FiringSyncFrame2
  effect: removed
credit:
- MentalHomiega
---

A vehicle's first burst round now waits for `FiringSyncFrame0` and its second for `FiringSyncFrame1`, the art keys Yuri's Revenge reads. We read `FiringSyncFrame1` and `FiringSyncFrame2` before. Rename `FiringSyncFrame1` to `FiringSyncFrame0` and `FiringSyncFrame2` to `FiringSyncFrame1`.
