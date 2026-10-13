---
title: Stop Tiberium deaths from leaving visceroids
category: fix
release: 0.2.0
targets:
- type: key
  id: TiberiumDeathToVisceroid
  effect: changed
- type: key
  id: SmallVisceroid
  scope: global-rules
  effect: changed
credit:
- MentalHomiega
---

Infantry killed by Tiberium no longer leave a small visceroid, whatever `TiberiumDeathToVisceroid` says. Yuri's Revenge does not read that switch during play, so the key is still read and saved but has no effect. Nothing spawns the `SmallVisceroid` type either.
