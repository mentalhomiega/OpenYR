---
key: DisabledDisguiseDetectionPercent
summary: "The percent chance, per side, that a computer house sees a disguised Mirage vehicle while it blinks."
see_also: [DisguiseWhenStill, DisguiseFakeBlinkTime, "system:disguises"]
when_omitted:
  kind: value
  value: none
---

A computer house can pick a [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle that looks like terrain to it only while the vehicle blinks after a shot, and then only when a roll from 0 to 99 is no more than this value. The values follow the side order: Allied, Soviet, then Third. A side without a value never sees through a blink. A human house never picks such a vehicle, and a type with [`DetectDisguise=yes`](/keys/detectdisguise/) sees through the disguise.

```ini title="rulesmd.ini"
[General]
DisabledDisguiseDetectionPercent=15,5,2
```
