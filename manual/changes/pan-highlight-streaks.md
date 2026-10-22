---
title: Stop highlight streaks trailing turrets while the map scrolls
category: fix
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
credit: [MentalHomiega]
---

With HDR highlights on, a turret that sat in view while the map scrolled left a streak of highlight on the ground behind it. The streak built up over the scroll and moved with the ground. Scrolling now clears the highlight in each strip the scroll reveals, so a highlight stays on its turret.
