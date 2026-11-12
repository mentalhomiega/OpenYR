---
key: ParallelPaths
scope: client-settings
label: Parallel paths
when_omitted:
  kind: value
  value: "no"
---

`ParallelPaths=yes` searches the paths of walking and driving units in batches on several processor threads, which shortens the longest frames when many units ask for a path at once. The key is read from the `[Options]` section of `RA2MD.INI`. The Performance page of the settings sets it with the switch **Parallel path searches**.

It applies to campaign and skirmish games started from the menus after the change. A game started from a launch file takes `ParallelPaths` from the `[Settings]` section of that file instead, and a multiplayer game started from the menus plays without it.

With it on, a path can differ slightly from the one the original game finds, because a path is searched from the map as it stands at the start or the end of the frame. A unit still starts moving in the frame in which it would have without the key. For example, 300 tanks ordered across the map get their paths in one batch spread over the threads.

[`PathThreads`](/keys/paththreads/) sets how many threads search.
