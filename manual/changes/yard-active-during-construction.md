---
title: Keep a construction yard active while the structure it placed is built
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

A construction yard now enters its active state when the structure it placed starts construction, and returns to guard when construction ends. Before, the yard lost its radio contact with the structure at placement, so it stayed idle through the whole build. While it is active, the yard's body draws the frames of its active sequence, which can include the damaged-looking frames of snow art. The start and end follow gamemd's construction mission (`BuildingClass::Mission_Construction`, 0x449A50) and repair mission (`BuildingClass::Mission_Repair`, 0x44B780).
