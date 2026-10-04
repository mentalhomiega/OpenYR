---
key: EnemyInsignia
summary: Whether players not allied with an object's owner see its rank insignia, unless the type decides.
see_also: [Insignia.ShowEnemy, "system:veterancy"]
when_omitted:
  kind: value
  value: "yes"
---

With `EnemyInsignia=yes`, a veteran, elite or below-rookie object shows its insignia to every player who can see it. With `no`, only its owner and the owner's allies see it. An observer sees every insignia either way.

[`Insignia.ShowEnemy`](/keys/insignia.showenemy/) on an object type overrides the setting for that type. [Rank display](/systems/veterancy/#rank-display) covers when the insignia is drawn at all.

```ini title="rulesmd.ini"
[General]
EnemyInsignia=no
```
