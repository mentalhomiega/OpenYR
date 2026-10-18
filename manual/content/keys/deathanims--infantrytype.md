---
key: DeathAnims
scope: infantrytype
label: Death animations
summary: The animation list that replaces this InfantryType's death sequence, indexed by the warhead's InfDeath value.
see_also: [DeadBodies, InfDeath, NotHuman]
when_omitted:
  kind: value
  value: ""
  note: The death sequence plays as usual.
---

`DeathAnims=` lists animations separated by commas. When a soldier of this type is killed, the entry at the position given by the killing warhead's [`InfDeath`](/keys/infdeath/) value appears at the soldier's position. The first entry is position `0`.

```ini title="rulesmd.ini"
[MYSOLDIER] ; example InfantryType
DeathAnims=MYSOLDIERGUN,MYSOLDIERBLAST
```

With that list, `InfDeath=1` shows `MYSOLDIERBLAST`. `InfDeath=0` shows `MYSOLDIERGUN`, and so does `InfDeath=2`, because a position past the end of the list shows the first entry. A negative position does the same.

A soldier killed while parachuting uses position `3`, whatever its warhead. One killed by a [laser fence](/systems/laser-fences/) uses position `5`.

The soldier is removed at once. No death sequence plays, so it leaves no [`DeadBodies`](/keys/deadbodies/#scope-infantrytype) corpse, and a [`NotHuman=yes`](/keys/nothuman/) type does not play its own death sequence either.
