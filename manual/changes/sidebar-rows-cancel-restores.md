---
title: Restore the saved sidebar rows when the Settings screen is cancelled
category: fix
release: 0.2.0
targets:
- type: key
  id: SidebarRows
  effect: changed
credit: [MentalHomiega]
---

Cancelling the Settings screen now puts back the saved [`SidebarRows`](/keys/sidebarrows/) value. Before, a rows value picked in the preview stayed on the build panel until the next save.
