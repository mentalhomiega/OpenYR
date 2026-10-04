---
title: Map zoom
summary: Zooms the map view with the mouse wheel between half and twice its normal size, leaving the sidebar unchanged.
category: interface-controls
keys: []
related:
  - type: key
    id: ZoomInFactor
  - type: command
    id: ScreenCapture
---

The mouse wheel over the map zooms the map view. Rolling forward zooms in and rolling back zooms out, by 0.1 for each notch. The zoom stays between 0.5 and 2.0. At 0.5 the view shows twice as much map in each direction, and at 2.0 the map is drawn twice its normal size. Notches that arrive together add up, and a notch past either limit does nothing.

Over the sidebar the wheel scrolls the sidebar, as before. Only the map's own area zooms: the sidebar, its tabs and the command bar keep their size.

## Where the view ends up

The point of the map under the cursor stays under the cursor while the zoom changes. Near the edge of the map the view cannot keep it there, and it slides to the nearest place the edge allows.

Each notch glides to the new zoom over 160 milliseconds. A notch during a glide continues from the zoom shown at that moment toward the new target.

## What zoom changes

Zoom changes only how the map is drawn. A game played at a changed zoom throughout matched a game without zoom on every game-state hash that was compared. Clicks, selection boxes and the cursor's target follow the map as drawn, and clicks outside the map's area are not map clicks.

When the game draws through its GPU presenter, the zoomed map is drawn as its own layer beneath the interface, at the window's resolution. With a game resolution of 1920 by 1080 on a 3840 by 2160 screen, the interface keeps its usual size and a zoom of 0.5 shows the map pixel for pixel. Without the presenter, the map is scaled into the game's picture at the game's resolution.

The [`ScreenCapture`](/commands/screencapture/) command saves the zoomed map along with the rest of the frame.

## Trigger zoom

[`ZoomInFactor`](/keys/zoominfactor/) and the zoom trigger actions are a separate, older zoom. The mouse wheel does not read `ZoomInFactor`.
