---
key: InitialPayload.Nums
scope: aircrafttype
summary: "How many of each InitialPayload.Types entry a new object carries."
see_also: ["InitialPayload.Types"]
when_omitted:
  kind: value
  value: "one of each type"
---

The numbers pair with the entries of [`InitialPayload.Types`](/keys/initialpayload.types/) in order. When the list is shorter, its last number applies to every entry after it. A negative or zero number puts none of that type inside.

```ini title="rulesmd.ini"
[SAPC] ; example VehicleType
InitialPayload.Types=E1,E2,ENGINEER
InitialPayload.Nums=2,1  ; two GIs, a Conscript and an Engineer
```
