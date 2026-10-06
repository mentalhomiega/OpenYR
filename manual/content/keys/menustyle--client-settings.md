---
key: MenuStyle
scope: client-settings
label: Menu style
when_omitted:
  kind: value
  value: Modern
---

`MenuStyle=` chooses how the menus look. It accepts two names, in any capitalization:

| Value | Result |
| --- | --- |
| `Modern` | Yuri's Revenge's title art fills the screen behind centered panels in Yuri's purple, with the text in a smooth face |
| `Classic` | Yuri's Revenge's own 800 by 600 menu screen, with the main and multiplayer buttons in the column down its right-hand side |

Both styles scale with the window. `Modern` is laid out for 1280 by 720, so it is drawn one and a half times as large at 1920 by 1080 and three times as large at 3840 by 2160. `Classic` is laid out for 800 by 600 and is centered, with black bars at the sides on a wide screen.

A name other than `Classic` is read as `Modern`, and the setting is written back to `RA2MD.INI` that way.

The display options screen has the same choice as its "Classic Yuri's Revenge menus" switch. Accepting that screen changes the style at once, and leaving the options menu saves the setting to `RA2MD.INI`.

The files each style draws with are in the [UI files](/systems/ui-files/#menu-styles).
