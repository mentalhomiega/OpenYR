---
key: EnemyWrench
summary: "With no, only a structure's owner, its allies and observers see the repair wrench over it."
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "yes"
---

`EnemyWrench` is an Ares key in `[General]`. A structure that its owner is repairing shows a blinking wrench. With `EnemyWrench=no`, a player who is not allied with the owner no longer sees that wrench, so the repair does not give away which structures the owner is paying to fix. Observers still see it. The repair itself is unchanged.

```ini title="rulesmd.ini"
[General]
EnemyWrench=no
```
