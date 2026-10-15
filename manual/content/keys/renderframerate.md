---
key: RenderFrameRate
summary: The most extra pictures per second the game draws between game frames.
when_omitted:
  kind: value
  value: "-1"
---

`RenderFrameRate=-1` is Match display. In solo and skirmish games the game draws between game frames at the refresh rate of the display, so moving objects look smooth. In a network game the setting means Off unless the file names a number. `RenderFrameRate=0` draws one picture for each game frame, as the original game did. A positive value lets the game draw between game frames, up to that many pictures per second, in any game. [High frame rate drawing](/systems/high-frame-rate-drawing/) describes what the extra pictures show and when they are skipped.

Match display uses the refresh rate that Windows reports for the display, or 60 when it reports none. Values below `-1` are read as `-1`, and values above `1000` as `1000`.

Each extra picture also reads input and runs the tactical map's AI before it draws. Input is therefore read between game frames as well as on them. This is why a network game keeps Off unless the file asks for a number.

Set a number to the refresh rate of the display, such as `60`. The game shows at most about one picture per display refresh (see [`VSync`](/keys/vsync/)), so a higher value draws pictures that are never shown.

The **Motion smoothing** list on the Display page of the settings offers Off, Match display, and 30, 60, 120, 144 and 240 pictures a second, and lists any other value the file holds. A change applies from the next game frame, and the game writes the value back to `RA2MD.INI` with the other options.

```ini title="RA2MD.INI"
[Video]
RenderFrameRate=-1
```
