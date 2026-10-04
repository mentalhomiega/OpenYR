---
title: High frame rate drawing
summary: Draws extra pictures between game frames that show moving objects part way along their move.
category: rendering-presentation
keys:
  - RenderFrameRate
  - VSync
---

With [`RenderFrameRate`](/keys/renderframerate/) above `0`, the game draws extra pictures while it waits for the next game frame. Each shows vehicles, infantry, aircraft and projectiles part way between where they stood in the previous game frame and where they stand in the current one. The facing of an object that is turning is blended the same way. Motion looks smooth at the chosen rate instead of moving in a step at each game frame.

Only drawing changes. In testing, game-state hashes matched a build that draws once per game frame.

## When a picture is drawn

The first extra picture is drawn as soon as the wait for the next game frame begins. Later ones follow at an interval of 1000 divided by `RenderFrameRate` milliseconds, rounded down and at least 1. During those pictures the game still reads input, so scrolling and clicks are handled between game frames.

A picture is skipped when the time left before the next game frame is no more than recent pictures took to draw. A game that draws slowly therefore gets fewer extra pictures, and the game frames themselves come on time.

An object that moved more than two cells in one game frame, for example one that teleported, is drawn at its current place without blending. A newly placed object is also drawn there for its first game frame.

## When it does not apply

No extra pictures are drawn while a replay plays back. While a dialog is open or the game window is out of focus, the wait is not used for extra pictures either.

## Display refresh

The game shows at most about one picture per display refresh, whatever the setting; see [`VSync`](/keys/vsync/). A `RenderFrameRate` above the refresh rate draws pictures that are not shown.
