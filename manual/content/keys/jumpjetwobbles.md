---
key: JumpjetWobbles
summary: "How often a jumpjet of this type bobs while it flies."
see_also: [WobblesPerSecond]
when_omitted:
  kind: value
  value: "[JumpjetControls] WobblesPerSecond"
---

Sets how many times a second a jumpjet of this type bobs up and down while it cruises or hovers, as [`WobblesPerSecond`](/keys/wobblespersecond/) describes. A type without the key uses `WobblesPerSecond` from `[JumpjetControls]`.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetWobbles=.01
```
