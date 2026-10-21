---
title: Map zoom
summary: Zooms the map view with the mouse wheel between half and twice its normal size, leaving the sidebar unchanged.
category: interface-controls
keys: []
related:
  - type: key
    id: ZoomInFactor
  - type: key
    id: ScriptedCameraZoom
  - type: key
    id: ScriptedCameraHeight
  - type: command
    id: ScreenCapture
---

The mouse wheel over the map zooms the map view. Rolling forward zooms in and rolling back zooms out, by 0.1 for each notch. The zoom stays between 0.5 and 2.0, or between 0.75 and 2.0 in a multiplayer game. At 0.5 the view shows twice as much map in each direction, and at 2.0 the map is drawn twice its normal size. Notches that arrive together add up, and a notch past either limit does nothing.

Over the sidebar the wheel scrolls the sidebar, as before. Only the map's own area zooms: the sidebar, its tabs and the command bar keep their size.

## How far out the zoom goes

On a small map the zoom out limit is higher. It is the zoom at which the map's visible area, the part the view scrolls over, fills the screen along one axis. At that zoom and further in, the view is no wider or taller than the visible area. The limit depends on the screen size.

If the map's visible area is smaller than the screen at normal size along either axis, the zoom out cannot go below normal size.

## Where the view ends up

The point of the map under the cursor stays under the cursor while the zoom changes. Near the edge of the map the view cannot keep it there, and it slides to the nearest place the edge allows.

Each notch glides to the new zoom over 160 milliseconds. A notch during a glide continues from the zoom shown at that moment toward the new target.

## What zoom changes

Zoom changes only how the map is drawn. A game played at a changed zoom throughout matched a game without zoom on every game-state hash that was compared. Clicks, selection boxes and the cursor's target follow the map as drawn, and clicks outside the map's area are not map clicks.

When the game draws through its GPU presenter, the zoomed map is drawn as its own layer beneath the interface, at the window's resolution. With a game resolution of 1920 by 1080 on a 3840 by 2160 screen, the interface keeps its usual size and a zoom of 0.5 shows the map pixel for pixel. Without the presenter, the map is scaled into the game's picture at the game's resolution.

The [`ScreenCapture`](/commands/screencapture/) command saves the zoomed map along with the rest of the frame.

## Scripted scenes

A scripted scene zooms the map view to [`ScriptedCameraHeight`](/keys/scriptedcameraheight/) map pixels tall, 900 by default, so the view shows the same height of map on any screen. The width follows the screen's aspect ratio. The change is made at once on the next frame, without the glide the wheel uses. Near the edge of the map the view slides to the nearest place the edge allows, as the wheel zoom does.

The scene zoom applies when a trigger action takes the input away (Lock input), moves the camera (Center Camera at Waypoint, Center (Jump) Camera at Waypoint, or Jump Camera Home), or starts the Chrono Screen Effect. A team mission that centers the viewpoint applies it as well.

How the player's zoom comes back depends on whether the action took the input:

- When the input is taken, the player's zoom is kept. It comes back when Unlock input runs, when the chronoshift effect that took the input ends, or when Zoom out runs with the input free. A second lock while the input is still taken keeps the first zoom.
- When the camera moves with the input free, the scene zoom stays. Nothing comes back, and the player zooms by hand.

While the input is taken, the mouse wheel does not zoom the map. The Zoom in trigger action is a separate effect and does not change the scene zoom.

[`ScriptedCameraZoom=no`](/keys/scriptedcamerazoom/) turns the scene zoom off. A save game does not hold the player's zoom. A game loaded in a locked scene keeps the zoom on screen when control returns.

## Trigger zoom

[`ZoomInFactor`](/keys/zoominfactor/) and the zoom trigger actions are a separate, older zoom. The mouse wheel does not read `ZoomInFactor`.
