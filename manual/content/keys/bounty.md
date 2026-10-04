---
key: Bounty
summary: Makes the house that owns this object collect a bounty whenever the object destroys an enemy object.
see_also: ["Bounty.Value", "Bounty.Display", BountyEnablers, GivesBounty, "system:bounty"]
when_omitted:
  kind: value
  value: "no"
---

With `Bounty=yes`, the object's owner is paid the [bounty](/systems/bounty/) of each enemy object this object destroys. The key is read on the destroying object. The amount comes from the destroyed object's [`Bounty.Value`](/keys/bounty.value/) keys, so an object that sets `Bounty=yes` earns nothing from a victim that sets no value keys.

The key applies to aircraft, structures, infantry and vehicles. [Bounty](/systems/bounty/) lists the other conditions a payment must meet.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType: its owner collects bounty for its kills
Bounty=yes

[MTNK] ; example VehicleType: pays 300 to the owner of whatever destroys it
Bounty.Value=300
```
