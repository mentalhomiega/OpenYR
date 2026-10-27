---
key: ShowFPS
scope: client-settings
label: Show FPS
when_omitted:
  kind: value
  value: "no"
---

`ShowFPS=yes` draws two counts in the top-left corner of the game view:

- `Render` is the number of pictures per second that reached the display in the last complete second. A picture is counted when it is shown. A picture the game draws too soon after the previous one, within one display refresh, is not shown and not counted.
- `Game` is the number of game ticks, the game frames the game ran, in the last 960 ms. It follows the game speed. Pictures drawn between game frames do not change it, so `Render` can be higher than `Game`. [`RenderFrameRate`](/keys/renderframerate/) sets how many pictures the game draws between game frames.

[`ShowFrameGraph`](/keys/showframegraph--client-settings/) adds a graph of recent frame times below the counts.

The count appears only during a game. The menus do not show it.

The **Show FPS** switch on the Display page of the settings sets the key, and the change applies at once.
