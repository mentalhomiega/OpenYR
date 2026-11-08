---
key: DisguiseWhenStill
summary: "Makes a vehicle look like a tree to other houses while it stands still."
see_also: [DefaultMirageDisguises, InfantryBlinkDisguiseTime, "system:disguises"]
when_omitted:
  kind: value
  value: "no"
---

A vehicle with this key [hides as terrain](/systems/disguises/#vehicles-that-hide-as-terrain) while it stands still.

```ini title="rulesmd.ini"
[MYMIRAGE] ; example VehicleType
DisguiseWhenStill=yes
```

The key is read on every object type, but only vehicles hide as terrain.
