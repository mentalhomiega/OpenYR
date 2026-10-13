---
title: Change the display and more game settings from the options
category: feature
release: 0.2.0
targets:
- type: key
  id: Fullscreen
  effect: changed
- type: key
  id: VSync
  effect: changed
- type: key
  id: ScaleMode
  effect: changed
- type: key
  id: IntegerScaling
  effect: changed
- type: key
  id: RenderFrameRate
  effect: changed
- type: key
  id: CursorScale
  effect: changed
- type: key
  id: Renderer
  effect: changed
- type: key
  id: AltToRally
  effect: changed
- type: key
  id: SimulateWhileUnfocused
  effect: changed
- type: key
  id: SidebarSorting
  effect: changed
- type: key
  id: AutoSaveInterval
  effect: changed
credit: [MentalHomiega]
---

The Display page of the settings can now be opened during a game, so the resolution and interface scale can be changed without leaving it. The page also offers [`Fullscreen`](/keys/fullscreen/), [`VSync`](/keys/vsync/), [`ScaleMode`](/keys/scalemode/), [`IntegerScaling`](/keys/integerscaling/), [`RenderFrameRate`](/keys/renderframerate/), [`CursorScale`](/keys/cursorscale/) and [`Renderer`](/keys/renderer/), which before could only be set in `RA2MD.INI`. All but the renderer apply at once. The Game page adds [`AltToRally`](/keys/alttorally/), [`SimulateWhileUnfocused`](/keys/simulatewhileunfocused/), [`SidebarSorting`](/keys/sidebarsorting/) and [`AutoSaveInterval`](/keys/autosaveinterval/). The Mods page stays on the main menu, because mods are loaded when a game starts.
