---
title: Keep the remaining players advancing when a player leaves an out-of-sync game
category: fix
release: 0.2.0
targets:
- type: system
  id: out-of-sync-recovery
  effect: changed
credit:
- MentalHomiega
---

When the master chose Continue in the out-of-sync dialog, or a player quit the dialog or stopped responding while it was open, the remaining players stopped advancing a few frames past the departed player's last frame and waited on the reconnect dialog. The remaining players now keep advancing after that player is removed.
