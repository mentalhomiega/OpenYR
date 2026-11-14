---
title: Restart the game from the Mods screen when the mod list changes
category: feature
release: 0.2.0
targets:
- type: command
  id: launch:wait-for-process
  effect: added
credit: [MentalHomiega]
---

When OK on the Mods screen saves a changed mod list, the game now offers **Restart now** beside **Later**. Restart now starts a new copy of the game with the options this one was started with and closes this one; the new copy gets `-WAITPID=` with this copy's process id and waits for it to go before it starts. Later keeps the old behaviour: the list takes effect the next time the game starts.
