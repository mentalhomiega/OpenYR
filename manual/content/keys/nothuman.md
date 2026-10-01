---
key: NotHuman
summary: Keeps an InfantryType from leaving one of the shared human corpses when it dies.
see_also: [DeadBodies, InfDeath]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an infantryman of this type that finishes a death sequence leaves a corpse only from his type's own [`DeadBodies`](/keys/deadbodies/#scope-infantrytype) list. With that list empty, he leaves none. With `no`, an empty type list falls back to the shared [`DeadBodies`](/keys/deadbodies/#scope-global-rules) corpses.

```ini title="rulesmd.ini"
[MYDOG] ; example InfantryType
NotHuman=yes
```
