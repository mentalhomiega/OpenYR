---
title: Draw sidebar cameos from PCX pictures
category: feature
release: 0.2.0
targets:
- type: key
  id: CameoPCX
  effect: added
- type: key
  id: SidebarPCX
  effect: added
credit:
- MentalHomiega
---

The sidebar can draw a PCX picture as a cameo. `CameoPCX=` in art.ini, in an object's image section, replaces the `Cameo=` shape on the sidebar strip, and `SidebarPCX=` in rules.ini does the same for a superweapon's button in place of `SidebarImage=`. A missing or unreadable file leaves the shape cameo in place. The key names follow Ares.
