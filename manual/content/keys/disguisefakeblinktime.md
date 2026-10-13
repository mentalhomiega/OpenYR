---
key: DisguiseFakeBlinkTime
summary: "How many frames a computer house's DisguiseWhenStill vehicle shows as a tank, and can be targeted, after it fires this weapon."
see_also: [DisguiseWhenStill, DisabledDisguiseDetectionPercent, "system:disguises"]
when_omitted:
  kind: value
  value: "0"
---

After a computer house's [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle fires this weapon, it looks like a tank to every viewer for this many frames, and its units may pick it as a target in those frames, subject to [`DisabledDisguiseDetectionPercent`](/keys/disableddisguisedetectionpercent/). Each shot starts the window again. With `0`, the shot opens no window.

```ini title="rulesmd.ini"
[MyMirageGun] ; example Weapon
DisguiseFakeBlinkTime=15
```
