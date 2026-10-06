---
key: Behind
scope: global-rules
label: 'Hidden object marker'
see_also: [ShowHidden, CanHideThings, CanBeHidden]
when_omitted:
  kind: value
  value: none
---

The animation drawn over an object hidden behind a [`CanHideThings=yes`](/keys/canhidethings/#scope-buildingtype) structure. Its frames from `LoopStart` to `LoopEnd` play in a loop at its `Rate`, centered on the object. It is drawn only while [`ShowHidden=yes`](/keys/showhidden/#scope-client-settings) is set.

When no `Behind` animation is set, blinking corner brackets are drawn around each hidden object instead, whatever `ShowHidden` says. An empty value keeps the animation set before it, so a map cannot switch back to the brackets once rulesmd.ini sets `Behind`.

```ini title="rulesmd.ini"
[General]
Behind=BEHIND ; an AnimType registered in [Animations]
```
