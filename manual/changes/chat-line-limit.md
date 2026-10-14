---
title: Limit an in-game chat line to 97 characters
category: fix
release: 0.2.0
targets:
- type: system
  id: chat
  effect: changed
credit:
- MentalHomiega
---

An in-game chat line now holds at most 97 characters, as in Yuri's Revenge, where it held 210. A longer line is sent in parts at a word break, as [In-game chat](/systems/chat/#long-lines) describes.
