---
key: NeverUse
summary: "Stops an object from ever choosing this weapon as its secondary."
when_omitted:
  kind: value
  value: "no"
---

An object whose secondary weapon has `NeverUse=yes` always [chooses its primary](/systems/target-selection/#which-weapon-the-score-assumes).

```ini title="rulesmd.ini"
[MyScannerWeapon] ; example WeaponType
NeverUse=yes
```
