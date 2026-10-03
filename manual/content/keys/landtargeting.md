---
key: LandTargeting
summary: "How the object attacks targets on land."
see_also: [NavalTargeting]
when_omitted:
  kind: value
  value: "0"
---

Sets how the object treats a target on land, which is anywhere other than water or a beach:

| Value | Effect |
| --- | --- |
| `0` | Normal [weapon choice](/systems/target-selection/#which-weapon-the-score-assumes). |
| `1` | The object does not fire at objects on the ground on land, or at land cells. |
| `2` | The object fires its secondary weapon at objects on the ground on land. A naval object also fires it at land cells. |

An object on a bridge counts as on land.

```ini title="rulesmd.ini"
[BSUB] ; Boomer
LandTargeting=2
```
