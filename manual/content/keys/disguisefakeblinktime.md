---
key: DisguiseFakeBlinkTime
summary: "How many frames a DisguiseWhenStill vehicle that the player does not control shows as a tank, and can be targeted, after it fires this weapon."
see_also: [DisguiseWhenStill, DisabledDisguiseDetectionPercent, "system:disguises"]
when_omitted:
  kind: value
  value: "0"
---

After a [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle that the player does not control fires this weapon, it looks like a tank to every viewer for this many frames. A computer house's units may pick it as a target in those frames, subject to [`DisabledDisguiseDetectionPercent`](/keys/disableddisguisedetectionpercent/). Each shot starts the window again. With `0`, the shot opens no window.

```ini title="rulesmd.ini"
[MyMirageGun] ; example Weapon
DisguiseFakeBlinkTime=15
```
