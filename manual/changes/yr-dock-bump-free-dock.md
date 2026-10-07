---
title: Leave docked aircraft in place while a dock is free
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitReload
  effect: changed
credit: [MentalHomiega]
---

At a `UnitReload=yes` pad with several docks, an aircraft that docks no longer sends the aircraft on the first dock away while it holds a dock or another dock is free. A repair bay with several docks also asks the docking object itself, not the one on the first dock, whether it needs repair.
