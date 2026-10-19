---
title: Read passenger seat offsets from AlternateFLH0
category: fix
release: 0.2.0
targets:
- type: system
  id: transports
  effect: changed
credit: [MentalHomiega]
---

The firing offsets of the passengers of an open-topped transport now come from the art keys `AlternateFLH0` to `AlternateFLH4`, as in Yuri's Revenge. They used to come from `AlternateFLH1` to `AlternateFLH5`, which left the first seat without its offset.
