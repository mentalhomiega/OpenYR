---
key: Fullscreen
summary: Whether the game covers the whole screen instead of running in a resizable window.
when_omitted:
  kind: value
  value: "yes"
---

With this setting on, the game covers the main display with a borderless window. With it off, the game opens an ordinary framed window that can be moved, resized, and maximized. [`WindowWidth`](/keys/windowwidth/) and [`WindowHeight`](/keys/windowheight/) set that window's starting size and are ignored in full screen.

Neither mode changes the desktop's resolution. The game renders at [`ScreenWidth`](/keys/screenwidth/) by [`ScreenHeight`](/keys/screenheight/) and scales that picture into its window, so switching away from the game leaves the desktop as it was.

The [`-WIN`](/using/command-line/windowed/) command line option opens a window for that run whatever this setting says. It does not change the stored value, so a launcher can offer a window without overriding the player's choice.

The **Fullscreen (borderless)** switch on the Display page of the settings changes the window at once, during a game too, and stores the choice. Turning it off opens a window of the `WindowWidth` and `WindowHeight` size, or of the rendering size where those are not set, kept inside the usable area of the display. While `-WIN` is in effect the switch starts off; turning it on stores `Fullscreen=yes`.
