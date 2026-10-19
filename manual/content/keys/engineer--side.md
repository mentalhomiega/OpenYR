---
key: Engineer
scope: side
label: Side survivor engineer type
see_also: ["system:capture", Crew, Crewed]
when_omitted:
  kind: computed
  note: The `[General]` `Engineer` type.
---

A structure that builds structures and that a house of the side owns, and has never been captured, leaves this type on the one-in-four roll that the [`[General]` key](/keys/engineer/#scope-global-rules) describes. A side that sets no type uses the `[General]` one, and a house with no side uses it too.

```ini title="rules.ini"
[FourthSide]
Engineer=FENGINEER
```
