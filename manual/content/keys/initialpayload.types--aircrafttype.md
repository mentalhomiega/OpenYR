---
key: InitialPayload.Types
scope: aircrafttype
summary: "The soldiers and vehicles a new transport, bunker or reactor already carries."
see_also: ["InitialPayload.Nums", Passengers, "Passengers.Allowed", CanBeOccupied, MaxNumberOccupants]
when_omitted:
  kind: value
  value: "none, so the object starts empty"
---

Each type named is created for the owner of the new object and put inside it, as many times as [`InitialPayload.Nums`](/keys/initialpayload.nums/) says, or once with no counts. The key applies to a vehicle, which takes passengers that fit by `Size`, `SizeLimit`, `Passengers=` and [`Passengers.Allowed`](/keys/passengers.allowed/), and to a structure, which takes soldiers only: those that an `Occupier` garrison holds in a `CanBeOccupied=yes` structure up to `MaxNumberOccupants=`, and those that an `InfantryAbsorb=yes` structure holds up to `Passengers=`. A passenger that does not fit is dropped, and so are the rest of its entry's copies.

A listed type that is an aircraft or a structure, or that carries a payload of its own, is skipped. Aircraft and soldiers carry no payload. The payload is made each time an object of the type is made, so a vehicle bought from the factory arrives loaded.

```ini title="rulesmd.ini"
[SAPC] ; example VehicleType
InitialPayload.Types=E1,E2
InitialPayload.Nums=2,1  ; two GIs and a Conscript
```
