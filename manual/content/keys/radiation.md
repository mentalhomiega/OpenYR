---
key: Radiation
summary: "Marks a warhead as radiation, which ImmuneToRadiation types ignore."
see_also: [ImmuneToRadiation, RadSiteWarhead, "system:radiation"]
when_omitted:
  kind: value
  value: "no"
---

An [`ImmuneToRadiation=yes`](/keys/immunetoradiation/) type takes no damage from a warhead set to `yes`.

```ini title="rulesmd.ini"
[MyRadSite] ; example Warhead
Radiation=yes
```
