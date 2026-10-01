---
key: NukeTakeOff
summary: "The animation a missile silo plays as its missile takes off."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

Plays where a missile silo's missile leaves the silo. [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile) covers the launch.

```ini title="rulesmd.ini"
[General]
NukeTakeOff=MYNUKETO ; an AnimType registered in [Animations]
```

With the key unset, the missile takes off without it.
