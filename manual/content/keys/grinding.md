---
key: Grinding
summary: Lets this structure take in its owner's infantry and vehicles and pay their refund.
see_also: [Soylent, RefundPercent, EnterGrinderSound, ReverseEngineersVictims]
when_omitted:
  kind: value
  value: "no"
---

A player who points one of their own infantrymen or vehicles at their `Grinding=yes` structure gets the enter cursor. The object heads for the structure, and when it reaches one of the structure's cells it is removed and its owner is paid its refund, as if it had been sold. A vehicle's passengers are removed with it, and their refunds are paid too. [`Soylent`](/keys/soylent/) replaces the refund where it is set.

```ini title="rulesmd.ini"
[MYGRINDER] ; example BuildingType
Grinding=yes
```

The structure takes any number of objects, one after another or at once. Aircraft cannot enter, and neither can another house's objects, allied or not.

Each object plays [`EnterGrinderSound`](/keys/entergrindersound/) where it is ground up. When the structure has an [`ActiveAnim`](/keys/activeanim/) running, that animation stops and its `SpecialAnim` starts in its place.

With [`ReverseEngineersVictims=yes`](/keys/reverseengineersvictims/), the structure also teaches its owner the type of each object it grinds up.
