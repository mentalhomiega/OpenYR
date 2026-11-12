---
key: PathThreads
scope: client-settings
label: Path threads
when_omitted:
  kind: value
  value: "0"
---

`PathThreads` is the number of processor threads that search paths while [`ParallelPaths`](/keys/parallelpaths/) is on, from 1 to 32. The default 0 uses one less than the number of threads the processor has, and at most 8. The key is read from the `[Options]` section of `RA2MD.INI`. The Performance page of the settings sets it with the slider **Path search threads**, whose lowest position reads Auto.

The number changes how long the searches take and never which paths are found. For example, `PathThreads=4` on a six-core processor leaves two cores to the rest of the game.
