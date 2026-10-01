---
key: MakesDisguise
summary: "Makes a warhead disguise its firer as the soldier it hits."
see_also: [CanDisguise, "system:disguises"]
when_omitted:
  kind: value
  value: "no"
---

When this warhead hits a soldier, a [`CanDisguise=yes`](/keys/candisguise/) firer [takes on that soldier's look](/systems/disguises/).

```ini title="rulesmd.ini"
[MySnapshot] ; example Warhead
MakesDisguise=yes
```
