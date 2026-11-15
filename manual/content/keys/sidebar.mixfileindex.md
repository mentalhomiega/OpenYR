---
key: Sidebar.MixFileIndex
summary: The number that names the sidebar archives a side plays with.
see_also: ["system:sidebar", "Sidebar.YuriFileNames"]
when_omitted:
  kind: computed
  note: The side's position in `[Sides]`, counted from 1, except that the third side uses 2, the number of the second.
---

A side whose section sets `Sidebar.MixFileIndex=3` mounts `SIDEC03MD.MIX`, `SIDEC03.MIX` and `SIDENC03.MIX` when the player plays that side, with the expansion copies `E<nn>SC03.MIX` and `E<nn>SNC03.MIX`. The number is written with two digits. The sidebar frame, the radar logo, the buttons and the build clock come from these archives, so the number picks the side's sidebar art in both the classic and the modern interface.

```ini title="rules.ini"
[FourthSide]
Sidebar.MixFileIndex=4
```

The key is read from the rules before the scenario's own rules, because the archives are mounted first. A value below 1 counts as omitted. A side that omits the key takes its number from its position in `[Sides]`: the first side mounts the `01` archives and the second the `02` archives. The third side shares the `02` archives with the second, as Yuri's Revenge does, and any later side uses its position plus 1. A mod with a fourth playable side therefore needs the key on any side that is not in the position its archives are numbered for.

The game stops loading the side when `SIDEC<nn>.MIX` is missing. The player's side then falls back to the first side's archives.
