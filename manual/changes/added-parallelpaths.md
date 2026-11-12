---
title: Add ParallelPaths to search unit paths on several threads
category: feature
release: 0.2.0
targets:
- type: key
  id: ParallelPaths
  effect: added
- type: key
  id: PathThreads
  effect: added
credit:
- MentalHomiega
---

[`ParallelPaths`](/keys/parallelpaths/) searches the paths of infantry and vehicles in batches on several processor threads, which shortens the longest frames in large battles. [`PathThreads`](/keys/paththreads/) sets the number of threads. The default `no` keeps every path as it was.
