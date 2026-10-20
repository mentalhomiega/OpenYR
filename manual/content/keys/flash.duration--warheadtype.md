---
key: Flash.Duration
scope: warheadtype
summary: "The frames an object flashes after this warhead hits it."
see_also: [AllowZeroDamage, EffectsRequireDamage, EffectsRequireVerses]
when_omitted:
  kind: value
  value: "0, so the warhead causes no flash"
---

A positive value makes a soldier, vehicle, aircraft or structure that the warhead hits flash for that many frames. The flash is the one a selected-object click or an elite promotion gives. It reaches an object that is alive when the hit lands and that the warhead's effects reach, which [`EffectsRequireVerses`](/keys/effectsrequireverses/) and [`EffectsRequireDamage`](/keys/effectsrequiredamage/) decide. A flash already running for longer is not shortened.

```ini title="rulesmd.ini"
[MyFlashWH] ; example WarheadType
Flash.Duration=40
```
