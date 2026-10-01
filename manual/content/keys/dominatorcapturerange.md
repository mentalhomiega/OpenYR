---
key: DominatorCaptureRange
summary: "How many cells around its target the psychic dominator takes units over."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "2"
---

Every unit that can be dominated on a cell within this many cells of the target joins the firing house. Distance counts the longer of the two axis distances plus half the shorter, and values above 10 count as 10.

```ini title="rulesmd.ini"
[General]
DominatorCaptureRange=1
```

At `0`, only the target cell is covered.
