---
key: EMP.Threshold
summary: The longest EM pulse stun this object survives; a longer stun destroys it.
see_also: [EMP.Modifier, ImmuneToEMP, Crashable, "system:emp-pulse"]
when_omitted:
  kind: value
  value: "no; a stun never destroys the object"
---

An EM pulse destroys the object when it stuns it for more frames than this value allows. The stun length is the pulse's duration after [`EMP.Modifier`](/keys/emp.modifier/) is applied. The accepted values are:

| Value | Meaning |
| --- | --- |
| `no` or `false`, or `0` | A stun never destroys the object. |
| `yes` or `true` | A stun of more than 1 frame destroys the object. |
| A number above `0` | A stun of more than that many frames destroys the object. |
| `inair`, or a number below `0` | A stun of more than `1` frame, or more than the number's size, destroys the object only if it is in the air when the pulse stuns it. |

A value that is not one of these words is read as a number, and text that is not a number counts as `0`.

The check runs only where a pulse stuns an object, so it never destroys an immune type. It does not apply to an aircraft in the air, which the pulse crashes or destroys as before; an aircraft standing on the ground is stunned and checked like a vehicle. The sweep stuns only objects on the ground and underground, so a negative value destroys only an object that is in the air at the moment it is stunned.

Every object in reach is stunned before any is destroyed. Each destroyed object takes damage equal to its full strength from [`C4Warhead`](/keys/c4warhead/), which skips armor and the Iron Curtain, and the pulse's firer is credited with the kill. A pulse with no firer credits no one.

```ini title="rulesmd.ini"
[APOC] ; example VehicleType that an EM pulse destroys
EMP.Threshold=yes
```
