---
key: FrameRateLimit
summary: The most presents per second the game makes. Zero is unlimited.
when_omitted:
  kind: value
  value: "-1"
---

`FrameRateLimit=-1` paces the presents to the refresh rate of the display, as the game has always done. A positive value sets the most presents per second. The game never presents more often than every 3 ms, so a value above about 333 behaves the same as 333. `FrameRateLimit=0` removes the limit, so the game presents each time the picture has changed. Any negative value means the display refresh.

A present that would come before the interval has passed is skipped, and the next present shows the newest picture. The limit does not pause the game: game frames run on their own timing.

With [`VSync`](/keys/vsync/) on, each present also waits for the display's refresh, so a limit above the refresh rate has no effect.

The **Frame rate limit** list on the Display page of the settings offers Display refresh, 30, 60, 120, 144, 240 and Unlimited, and lists any other value the file holds. The change applies when the options screen closes, and the game writes the value back to `RA2MD.INI` with the other options.

```ini title="RA2MD.INI"
[Video]
FrameRateLimit=-1
```
