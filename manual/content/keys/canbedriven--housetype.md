---
key: CanBeDriven
scope: housetype
summary: "Whether CanDrive soldiers can take over vehicles that this country owns."
see_also: [CanDrive, MultiplayPassive]
when_omitted:
  kind: computed
  note: "the value of MultiplayPassive"
---

With `CanBeDriven=yes`, a soldier with [`CanDrive=yes`](/keys/candrive/) can take over this country's vehicles, if their types allow it. With `no`, it cannot. This lets a map place neutral vehicles that cannot be taken.

```ini title="rulesmd.ini"
[Neutral] ; example Country
CanBeDriven=no
```
