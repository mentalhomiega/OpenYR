---
key: InterfaceStyle
scope: client-settings
label: In-game interface style
when_omitted:
  kind: value
  value: Modern
---

`InterfaceStyle=` chooses how the in-game interface is laid out. It accepts two names, in any capitalization:

| Value | Result |
| --- | --- |
| `Classic` | Yuri's Revenge's sidebar down one side of the screen, with the command bar along the bottom |
| `Modern` | The map fills the screen. The radar, credits, power and build cameos sit in one framed panel in the top-right corner, three cameos to a row and [`SidebarRows`](/keys/sidebarrows/) rows deep (four by default), ending under the scroll arrows so the map shows below it. The command bar sits in a small frame in the bottom-left corner. Build tooltips show beside the panel |

A name other than `Modern` is read as `Classic`, and the setting is written back to `RA2MD.INI` that way.

The display options screen has the same choice as its "Modern in-game interface" switch. A change made from the main menu takes effect when the screen closes with OK. A change made during a game takes effect when the game returns to the main menu, and the screen says so under the switch. The menu battle behind the main menu runs only with the modern style.
