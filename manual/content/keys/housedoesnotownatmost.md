---
key: HouseDoesNotOwnAtMost
summary: Whether trigger event 82 holds while the trigger's house owns exactly the number the event names.
when_omitted:
  kind: value
  value: "yes"
  note: Mental Omega's campaign maps rely on `yes`.
---

`HouseDoesNotOwnAtMost` is our key, read from `[General]`. It sets the comparison of trigger event 82, House Doesn't Own Techno Type, which names a number and an object type:

| Value | The event holds while the trigger's house owns |
|---|---|
| `yes` | the number or fewer |
| `no` | fewer than the number |

The Ares manual words event 82 as `no`. Mental Omega's maps use the event with a number of 0 to mean that the house owns none, and pair it with event 81 at the same number to test for an exact count; both work only with `yes`. Which comparison Ares makes was not observed.

```ini title="rulesmd.ini"
[General]
HouseDoesNotOwnAtMost=no
```
