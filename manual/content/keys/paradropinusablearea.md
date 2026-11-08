---
key: ParadropInUsableArea
summary: "Whether a paratrooper lands only on a cell inside the usable map area."
see_also: [ParadropRadius, "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

A paratrooper whose landing cell lies outside the usable map area stays aboard its plane, and the plane tries again at its next drop check, 5 frames later. With `no`, a paratrooper lands on any cell it can enter, including one outside the usable area.

```ini title="rulesmd.ini"
[General]
ParadropInUsableArea=no
```

The landing cell is the one half a cell to one side of the plane's path. The key changes only the check on that cell. The plane still starts its drops only while it is over the usable area and within [`ParadropRadius`](/keys/paradropradius/) of its target.
