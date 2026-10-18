---
title: Stop meteors from cratering terrain by default
category: fix
release: 0.2.0
targets:
- type: key
  id: CraterLevel
  effect: changed
credit:
- MentalHomiega
---

A meteor no longer deforms the terrain under its impact unless a mod or map sets [`CraterLevel`](/keys/craterlevel/) above `0`. The default is now `0`, as in the original game. The stock rules file's `CraterLevel=1` no longer turns craters on.
