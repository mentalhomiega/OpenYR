---
title: Use the Windows pointer by default
category: feature
release: 0.2.0
targets:
- type: key
  id: SystemCursor
  effect: changed
credit:
- MentalHomiega
---

[`SystemCursor`](/keys/systemcursor--client-settings/) now defaults to `yes`, so menus and dialogs show the Windows mouse pointer. The battlefield pointers stay the game's own. A settings file that already sets `SystemCursor` keeps its value.
