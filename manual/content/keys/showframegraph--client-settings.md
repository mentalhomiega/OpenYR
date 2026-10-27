---
key: ShowFrameGraph
scope: client-settings
label: Show frame graph
when_omitted:
  kind: value
  value: "no"
---

`ShowFrameGraph=yes` draws a graph of recent frame times under the [`ShowFPS`](/keys/showfps--client-settings/) counts. It has no effect unless `ShowFPS=yes`.

The graph has one column for each of the last 120 gaps between presents, the pictures the display shows. The newest column is at the right edge. A column is 1 pixel for each millisecond of its gap, and a gap of 40 ms or more fills the graph's height. The label to the right of the graph gives that 40 ms scale. The gaps come from `timeGetTime`, so each one is a whole number of milliseconds.

A gap includes any time with no present, such as an idle screen or a minimised window, so that time appears as one long column.
