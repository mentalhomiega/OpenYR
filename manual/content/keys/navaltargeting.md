---
key: NavalTargeting
summary: "How the object attacks targets on water."
see_also: [LandTargeting, Underwater]
when_omitted:
  kind: value
  value: "0"
---

Sets which weapon the object uses against an object on water or a beach that is not in the air or on a bridge, or whether it may fire at it at all:

| Value | Effect |
| --- | --- |
| `0` | Primary weapon. The object does not fire at an [`Underwater=yes`](/keys/underwater/) target unless it is fully visible. |
| `1` | Secondary weapon against an `Underwater=yes` target, primary against the rest. |
| `2` | Primary weapon against an `Underwater=yes` target. The object does not fire at the rest. |
| `3` | Secondary weapon against an `Organic=yes` or `Unnatural=yes` target, primary against the rest. |
| `4` | Primary weapon against a hovering or `Organic=yes` target, secondary against the rest. |
| `5`, `7` | Primary weapon. |
| `6` | The object does not fire at targets on water. |

```ini title="rulesmd.ini"
[DEST] ; Destroyer
NavalTargeting=1
```
