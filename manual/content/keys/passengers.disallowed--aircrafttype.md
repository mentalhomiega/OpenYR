---
key: Passengers.Disallowed
scope: aircrafttype
summary: "The passenger types this transport refuses."
see_also: ["Passengers.Allowed", "Passengers.BySize"]
when_omitted:
  kind: value
  value: "none, so no type is refused for its name"
---

A type listed here never boards the transport, whatever its size and whatever [`Passengers.Allowed`](/keys/passengers.allowed/) says.

```ini title="rulesmd.ini"
[APC] ; example VehicleType
Passengers.Disallowed=ENGINEER
```
