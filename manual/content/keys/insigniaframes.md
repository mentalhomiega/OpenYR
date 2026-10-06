---
key: InsigniaFrames
summary: The insignia frames of an object type's rookie, veteran and elite ranks, as a comma-separated list.
see_also: [InsigniaFrame, Insignia, "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: Each rank uses InsigniaFrame, its own rank key, or the stock frame.
---

`InsigniaFrames` lists the frames for the rookie, veteran and elite ranks, in that order. Each is read as a whole number and written like [`InsigniaFrame`](/keys/insigniaframe/), where a value below `0` selects the stock frame. A list shorter than three values sets only the ranks it reaches. A list that does not start with a number sets none.

The list replaces `InsigniaFrame` for the ranks it sets. `InsigniaFrame.Rookie`, `InsigniaFrame.Veteran` and `InsigniaFrame.Elite` then override it for their own rank.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
InsigniaFrames=0,1,2 ; rookie frame 0, veteran frame 1, elite frame 2
```
