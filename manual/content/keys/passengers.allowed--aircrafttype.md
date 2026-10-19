---
key: Passengers.Allowed
scope: aircrafttype
summary: "The only passenger types this transport takes."
see_also: ["Passengers.Disallowed", "Passengers.BySize", SizeLimit, NoManualEnter]
when_omitted:
  kind: value
  value: "none, so any type that fits is taken"
---

With at least one name listed, the transport takes a passenger of those types and refuses every other. To refuse everyone, list a type no one can be, such as a structure. The refusal is the same as for a passenger that is too large: the enter cursor is not shown, and no order or script puts the passenger aboard. [`Passengers.Disallowed`](/keys/passengers.disallowed/) wins when a type is in both lists.

```ini title="rulesmd.ini"
[FV] ; example VehicleType
Passengers.Allowed=E2,ENGINEER
```
