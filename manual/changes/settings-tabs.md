---
title: Show the options as the tabs of one Settings screen
category: feature
release: 0.2.0
targets:
- type: key
  id: MenuStyle
  effect: changed
- type: system
  id: ui-files
  effect: changed
credit: [MentalHomiega]
---

In the modern menu style, Options on the main menu and Game Controls in the in-game options menu now open one Settings screen with a row of tabs along the top: Game, Display, Audio, Keyboard and Mods. Over a game in progress only Game, Audio and Keyboard are offered. We kept each tab as the screen it was, so it applies as it did before: choosing another tab applies the page you are leaving as OK does, OK applies it and leaves the Settings, and Back (Escape) leaves without applying it, except on the Game tab, which keeps what you set as before. Ctrl+Tab and Ctrl+Page Up and Page Down move between the tabs, and the arrow keys do when a tab has the focus. The classic menu style keeps the separate screens.
