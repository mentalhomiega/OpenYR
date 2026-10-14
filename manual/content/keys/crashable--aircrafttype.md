---
key: Crashable
scope: aircrafttype
label: Falls when destroyed
see_also: [CrashingSound, "system:emp-pulse", "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "yes"
---

With `Crashable=no`, an aircraft destroyed in the air does not fall. It is destroyed where it flies, as an aircraft on the ground is, and does not play its [`CrashingSound`](/keys/crashingsound/). Its passengers die with it.

An [EMP pulse](/systems/emp-pulse/#aircraft) that catches the aircraft destroys it with damage equal to its full strength from [`C4Warhead`](/keys/c4warhead/), unless its type is immune. A type with `Crashable=yes` crashes instead, or is only stunned when it stands on the ground. An ion storm does not crash the aircraft.

```ini title="rulesmd.ini"
[ORCA] ; example AircraftType that is destroyed in place
Crashable=no
```
