---
key: JumpJetTurn
scope: infantrytype
label: 'Turning to face a target while hovering'
summary: "Makes a jumpjet soldier turn its body toward its target while it hovers, and hold its fire while it is flying between places."
see_also: [JumpJet, Locomotor, JumpjetTurnRate]
when_omitted:
  kind: value
  value: "no"
---

With `JumpJetTurn=yes`, a jumpjet soldier that hovers over a target turns its body toward it. While the soldier flies somewhere, it is drawn facing the way it flies, whatever it is aiming at. It does not fire while it is climbing, cruising or descending, and shoots only once it is hovering or on the ground.

With the default `no`, a soldier with a target is drawn facing that target, even while it flies away from it.

```ini title="rulesmd.ini"
[LUNR] ; Lunar Rocketeer
JumpJet=yes
Locomotor={92612C46-F71F-11d1-AC9F-006008055BB5}
JumpJetTurn=yes
```

The key applies only while the soldier's [`JumpJet=yes`](/keys/jumpjet/) type has the jumpjet locomotor in control, so a jumpjet soldier that walks ignores it.
