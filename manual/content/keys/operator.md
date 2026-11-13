---
key: Operator
summary: "Shuts an object down while none of the listed types, or no passenger at all, is aboard."
see_also: [PoweredBy, Passengers]
when_omitted:
  kind: value
  value: "empty, so the object needs no operator"
---

`Operator` is an Ares key. It takes a comma-separated list of infantry or vehicle type names, or the word `_ANY_`. An object that names an operator works only while a passenger it accepts is aboard. With `_ANY_` any passenger counts; otherwise only a passenger of a listed type does. Without one the object shuts down: it drops its target, and a vehicle also drops its orders and stops. It starts again as soon as an accepted passenger boards. The word `_ANY_` may be mixed with type names.

A name that matches no type is skipped, and a note is written to the debug log. The word `none` is accepted and does nothing. If the key is present with a value, it replaces whatever list the type held before. An empty value leaves the list as it was.

Shutting down uses the same deactivate and activate sounds as [`PoweredBy`](/keys/poweredby/), and an object with both keys must satisfy both.

```ini title="rulesmd.ini"
[FV]
Operator=E1
```
