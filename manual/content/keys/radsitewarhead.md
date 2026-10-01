---
key: RadSiteWarhead
summary: "The warhead radiation damage goes through."
see_also: [RadLevelFactor, Radiation, "system:radiation"]
when_omitted:
  kind: value
  value: none
---

Radiation [damage](/systems/radiation/#damage) uses this warhead, so its `Verses` decide how much each armor takes. With none set, radiation does no damage.

```ini title="rulesmd.ini"
[Radiation]
RadSiteWarhead=MyRadSite ; a Warhead section
```
