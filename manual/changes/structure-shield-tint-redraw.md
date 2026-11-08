---
title: Tint a structure's body under the Force Shield, and redraw it while the Iron Curtain holds
category: fix
release: 0.2.0
targets:
- type: key
  id: ForceShieldColor
  effect: changed
credit: [MentalHomiega]
---

A structure under a Force Shield is drawn with `ForceShieldColor` on its body as well as its animations. Before, the body kept its untinted picture, because it was not drawn again when the shield came on or went off. A structure under the Iron Curtain is drawn again every frame, so its brightness pulse shows on the body and on its animations. The body and animations return to normal when the curtain ends.
