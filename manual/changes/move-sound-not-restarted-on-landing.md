---
title: Do not restart a MoveSound loop when a limboed object lands
category: fix
release: 0.2.0
targets:
- type: key
  id: MoveSound
  effect: changed
credit: [MentalHomiega]
---

An object that leaves the map while its [`MoveSound`](/keys/movesound/) loop is playing, such as a Terror Drone riding its shot, no longer starts the loop again when it lands while still moving. The loop stays silent until the object has stopped for a few game frames. Before, the landing frame started a new loop.
