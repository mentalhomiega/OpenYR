---
key: MoveToCellWide
scope: global-rules
label: Wide cell numbers in Move to Cell script lines
see_also: [NewINIFormat]
when_omitted:
  kind: computed
  note: yes when the rules declare any armor type in an Ares `[ArmorTypes]` section, otherwise no.
---

Whether a Move to Cell team script line on a map with `NewINIFormat=4` or higher gives its cell as `1000 * Y + X`.

- With `no`, the cell is `128 * Y + X`, as Yuri's Revenge reads it on every map. Cell 75,62 is `8011`.
- With `yes`, the cell is `1000 * Y + X`, as Phobos documents for maps with `NewINIFormat=4` or higher. Cell 75,62 is `62075`.
- A map with a lower `NewINIFormat` always uses `128 * Y + X`.

```ini title="rulesmd.ini"
[General]
MoveToCellWide=yes
```

This is our own key. Every stock map says `NewINIFormat=4` but none has a Move to Cell line, so `NewINIFormat` alone cannot tell which form a map uses. Rules that declare Ares armor types read the wide form, which Phobos-based mods expect, and stock rules keep the Yuri's Revenge form. A number that gives a cell outside the map leaves the line without a target.
