---
key: Crashable
summary: Whether an aircraft of this type falls and crashes when it is destroyed in the air.
see_also: [CrashingSound, "system:emp-pulse", "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "yes"
---

With `Crashable=no`, an aircraft destroyed in the air does not fall. It is destroyed where it flies, as an aircraft on the ground is, and does not play its [`CrashingSound`](/keys/crashingsound/).

An [EMP pulse](/systems/emp-pulse/#aircraft) that catches the aircraft destroys it with damage equal to its full strength from [`C4Warhead`](/keys/c4warhead/), unless its type is immune. A type with `Crashable=yes` crashes instead, or is only stunned when it stands on the ground. An ion storm does not crash the aircraft.

Only an AircraftType uses the key; the other object types read it and ignore it.

```ini title="rulesmd.ini"
[ORCA] ; example AircraftType that is destroyed in place
Crashable=no
```
