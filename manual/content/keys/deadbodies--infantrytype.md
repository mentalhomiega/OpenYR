---
key: DeadBodies
scope: infantrytype
label: Corpse animations
summary: The corpses this InfantryType leaves in place of the shared list.
see_also: [NotHuman, InfDeath]
when_omitted:
  kind: value
  value: ""
  note: The shared [General] list is used, unless the type sets NotHuman=yes.
---

When an infantryman of this type finishes a death sequence, one animation from this list appears at his center, each entry equally likely. With the list empty, he leaves one of the shared [`DeadBodies`](/keys/deadbodies/#scope-global-rules) corpses instead, or none when the type sets [`NotHuman=yes`](/keys/nothuman/).

```ini title="rulesmd.ini"
[MYDOG] ; example InfantryType
DeadBodies=MYDOGDIE
```
