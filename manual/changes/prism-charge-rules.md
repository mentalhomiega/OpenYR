---
title: Match the prism and tesla charge rules of Yuri's Revenge
category: fix
release: 0.2.0
targets:
- type: system
  id: prism-towers
  effect: changed
credit:
- MentalHomiega
---

A prism tower or tesla coil that is charging a shot now reports itself as still reloading, so its attack mission waits two frames between checks until the charge ends. A tower is recruited as support unless it is rearming, charging, drained or attacking; low power and stun no longer rule it out, and distance is the straight line between the towers. The damage bonus rounds as the original game does, and a shot that cannot be fired leaves the count of supporting towers unchanged. A structure that is still being built up or sold, or is drained, cannot fire.
