---
title: Swirl the chronoshift screen effect and lock input
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: changed
credit:
- MentalHomiega
---

The Chrono Screen Effect action now twirls the picture as Yuri's Revenge does, instead of only fading it to white. The picture swirls as it stood when the action ran, whitens over the given frames, stays white for 45 frames, then swirls back from the picture as it stands after the white. The player's input is locked while the effect runs. Before, the effect only faded and left the input alone.
