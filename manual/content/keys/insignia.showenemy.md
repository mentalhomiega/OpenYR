---
key: Insignia.ShowEnemy
summary: Whether a player not allied with the owner sees this object type's rank insignia.
see_also: [EnemyInsignia, Insignia, "system:veterancy"]
when_omitted:
  kind: inherited
  note: "[General] EnemyInsignia, which is yes when it is also absent."
---

With `Insignia.ShowEnemy=yes`, a veteran, elite or below-rookie object of this type shows its insignia to every player. With `no`, only the owner and the owner's allies see it, and a player of any other house does not. An observer sees every insignia either way. The key replaces [`EnemyInsignia`](/keys/enemyinsignia/) for this type, whichever way it is set.

```ini title="rulesmd.ini"
[YENGINEER] ; example InfantryType: its rank stays hidden from enemies
Insignia.ShowEnemy=no
```
