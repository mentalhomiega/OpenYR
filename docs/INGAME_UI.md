# Modern in-game interface

This is the plan for the modern build's in-game interface: the map fills the screen, and the
controls sit in compact framed panels over it, in the style of OpenRA. The classic Yuri's Revenge
sidebar stays available as an option, as the classic menus do.

## What we are aiming for

At 1920x1080:

- **The map fills the whole screen.** There is no sidebar column and no strip kept for the command
  bar; panels draw over the map, and the map scrolls under them.
- **The build panel** sits in the top-right corner, 250 pixels wide, framed in the side's colour
  (blue for the Allies, red for the Soviets, purple for Yuri). From the top:
  - a row of buttons: place beacon, sell, power down and repair, then the options button. Place
    beacon toggles beacon placement, as the `PlaceBeacon` command does; in a single-player or
    skirmish game it does nothing, as in gamemd. Power down shows "Coming soon" and does nothing
    yet. There is no diplomacy button;
  - the radar, whose map picture fills its box as far as the map's shape allows;
  - the credits on the left and the power used and the power made on the right, each after a small
    symbol: a gold dollar sign, and a lightning bolt that turns from green to red when the base uses
    more power than it makes;
  - the build area: the power bar on the left edge, the category tabs in a column beside it, and
    the cameos of the chosen tab in a grid of three columns. Each cameo carries its name and, while
    it builds, the countdown shading. The grid has as many rows as the chosen tab's cameos need,
    at least four and no more than the screen allows, with the scroll buttons under it. The panel
    grows and shrinks with the grid.
- **The command bar** is a slim framed strip in the bottom-left corner, holding the [AdvancedCommandBar]
  buttons as icons in the same style as the panel's buttons. It is only as wide as the buttons the
  list names, and it has no end cap, so it cannot be closed. A button's tooltip, its `Tip:` label
  from the string table as in Yuri's Revenge, shows just above the bar.
- **Superweapon timers** and the mission timer stack in the bottom-left corner, above the command
  bar, one line each in the owner's colour.
- **Messages** (chat, EVA text) keep their place at the top left of the map.
- **Tooltips** for the build panel show on the map to the panel's left, level with the pointer,
  in a frame of the side's colour.

A tab with nothing to build is shown dimmed, as in Yuri's Revenge, and a tab whose cameo has
finished building flashes.

## How it is built

The engine draws the screen in two layers already. The tactical map is drawn into the composite
surface, and while the map is zoomed the presenter draws the map underneath the composite at the
window's own resolution, with the map's part of the composite keyed out. The modern interface uses
the same arrangement whether or not the map is zoomed:

1. **View size.** With the modern interface on, the tactical view is the whole screen
   (`Set_View_Dimensions` with the full visible rectangle), and the surfaces are allocated for it.
   The sidebar surface becomes the build panel's own surface, sized to the panel.
2. **Drawing.** `SidebarClass::Draw_It`, `RadarClass`, `PowerClass` and `TabClass` keep drawing into
   the sidebar surface, laid out for the panel instead of the classic column. After the map's
   foreground pass, `GScreenClass::Render` copies the panel onto the composite at its place in the
   top-right corner, so it sits over the map in both layers. The command bar and the superweapon
   timers draw straight onto the composite after the map. The classic `Blit_Sidebar` copy to the
   visible surface is skipped.
3. **Frames.** The panel frames are drawn by code (filled boxes, borders and corner cuts in the
   side's colours from a small table), not from SIDEGDI shapes, so they fit any panel size. The
   cameos, power pips and radar keep the game's own art. The tab, button and scroll icons are
   PNG sheets in the `ui` folder, one set per side, named `HUD` plus the side letter (`A`, `S`
   or `Y`) and the icon's code, such as `HUDSPW.png` for the Soviet power-down button. The command
   bar's codes are `C1` to `C3` for the teams, then `TS`, `DP`, `AM`, `GD`, `BC`, `ST`, `PL` and `CH`
   in [AdvancedCommandBar] order; the beacon shares the panel's sheet. Each sheet
   holds three square frames: normal, pressed and disabled. `tools/hudicons/make_hud_icons.py`
   draws the current set. A panel button whose sheet is missing keeps Yuri's Revenge's art; the
   command bar keeps Yuri's Revenge's art for every button unless each listed button has a sheet.
4. **Input.** The panel and the command bar are hit-tested before the map: a click inside a panel's
   rectangle goes to the panel's buttons and never reaches the map, and the mouse over a panel shows
   the normal arrow. Edge scrolling stays at the screen edges.
5. **Layout.** One function computes every panel rectangle from the screen size (and later a UI
   scale), and everything else reads those rectangles, so a resize or a change of side only reruns
   it.

## Option

`[Options] InterfaceStyle=Modern|Classic` in the user settings, defaulting to Modern in the modern
build once it is complete, and to Classic until then. The options screen gets a matching choice.
Changing it takes effect at the next game start.

## Steps

1. **Full-screen map with the classic sidebar floating over it.** The view takes the whole screen;
   the existing sidebar is copied onto the map at the top right and hit-tested first. This proves the
   drawing and input changes with art that already works.
2. **The build panel layout.** Code-drawn frames; the icon row; the radar and power bar; the credits,
   time and power strip; tabs in a column; three cameo columns with names; scrolling.
3. **The command bar** in the bottom-left corner.
4. **Superweapon timers** in the bottom-left corner.
5. **Polish:** side colours, hover and pressed states, flashing tabs, tooltips placed beside the
   panel, UI scale for high resolutions, and the options screen choice.

Each step is checked in game with screenshots from the autotest runner (`-ShotDir`) at 1920x1080
and at 1280x720, plus the campaign wins, since every mission is played through this interface.

## Open questions

- **Cameo size.** The game's cameos are 60x48, so three columns make a panel 250 pixels wide.
  `[Video] InterfaceScale=` enlarges the panels with the rest of the frame (2x at 2160 lines), while
  the map keeps the screen's own detail. Whether 1080p wants a 1.5x panel, closer to OpenRA's look,
  is still open.
- **Radar size.** The radar's map picture is scaled to fit its box and centred in it, so a square
  map leaves dark bars at the sides. Whether the box should follow the map's shape is still open.
- **Classic art option.** Whether the modern layout should also offer the Yuri's Revenge sidebar
  art (SIDEGDI) as its frames, instead of code-drawn ones.
