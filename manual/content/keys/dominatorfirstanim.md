---
key: DominatorFirstAnim
summary: "The animation that builds up a psychic dominator blast."
see_also: [DominatorSecondAnim, DominatorFireAtPercentage, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays over the psychic dominator's target when it is fired. The blast fires once it has played [`DominatorFireAtPercentage`](/keys/dominatorfireatpercentage/) percent of its frames.

```ini title="rulesmd.ini"
[General]
DominatorFirstAnim=MYDOMCLOUD ; an AnimType registered in [Animations]
```

With this key or `DominatorSecondAnim` unset, the dominator does nothing.
