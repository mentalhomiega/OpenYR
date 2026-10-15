---
key: VSync
summary: Whether the renderer waits for the display's refresh before showing a finished frame.
when_omitted:
  kind: value
  value: "yes"
---

With `VSync=yes`, each present waits for the display's next refresh. This removes tearing, the horizontal seam that can appear while the map scrolls.

The wait adds delay between an input, such as a scroll, and seeing its result, because the game loop pauses for each present. The mouse pointer moves without that delay, because the system draws it over the game's picture.

Whatever this setting says, the game presents no more often than [`FrameRateLimit`](/keys/frameratelimit/) allows. By default that is once per display refresh, using the refresh rate Windows reports for the display, or 60 Hz when none is reported. A present that would come too early is skipped, and the next present shows the newest picture.

The **V-Sync** switch on the Display page of the settings changes it at once, during a game too.
