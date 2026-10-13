---
key: RenderFrameRate
summary: The most extra pictures per second the game draws between game frames.
when_omitted:
  kind: value
  value: "0"
---

`RenderFrameRate=0` draws one picture for each game frame, as the original game did. A positive value lets the game also draw between game frames, up to that many pictures per second, so moving objects look smooth. [High frame rate drawing](/systems/high-frame-rate-drawing/) describes what the extra pictures show and when they are skipped.

Values below `0` are read as `0` and values above `1000` as `1000`. The setting changes only what is drawn; the game itself runs the same.

Set it to the refresh rate of the display, such as `60`. The game shows at most about one picture per display refresh (see [`VSync`](/keys/vsync/)), so a higher value draws pictures that are never shown.

The **Smooth motion** list on the Display page of the settings offers Off and 30, 60, 120, 144 and 240 pictures a second, and lists any other value the file holds. A change applies from the next game frame, and the game writes the value back to `RA2MD.INI` with the other options.

```ini title="RA2MD.INI"
[Video]
RenderFrameRate=60
```
