---
key: DisguiseFakeBlinkTime
summary: "How many frames a DisguiseWhenStill vehicle stays open to computer targeting after it fires this weapon."
see_also: [DisguiseWhenStill, DisabledDisguiseDetectionPercent, "system:disguises"]
when_omitted:
  kind: value
  value: "0"
---

After a [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle fires this weapon, a computer house may pick the vehicle as a target for this many frames, subject to [`DisabledDisguiseDetectionPercent`](/keys/disableddisguisedetectionpercent/). Each shot starts the window again. With `0`, the shot opens no window.

```ini title="rulesmd.ini"
[MyMirageGun] ; example Weapon
DisguiseFakeBlinkTime=15
```
