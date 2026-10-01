---
key: DominatorSecondAnim
summary: "The animation of a psychic dominator blast."
see_also: [DominatorFirstAnim, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays over the psychic dominator's target as the blast fires. A computer house does not fire another dominator until this animation has ended.

```ini title="rulesmd.ini"
[General]
DominatorSecondAnim=MYDOMBLAST ; an AnimType registered in [Animations]
```

With this key or `DominatorFirstAnim` unset, the dominator does nothing.
