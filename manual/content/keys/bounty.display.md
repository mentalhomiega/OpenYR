---
key: Bounty.Display
summary: Whether the bounty this object earns its owner is shown over the destroyed object.
see_also: [BountyDisplay, Bounty, "system:bounty"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] BountyDisplay, which is no when it is also absent."
---

The key is read on the destroying object. With `Bounty.Display=yes`, each payment this object earns its owner shows as `+$300` rising over the spot where the destroyed object stood. With `no`, none of this object's payments are shown, whatever [`BountyDisplay`](/keys/bountydisplay/) says.

A payment of 0 is never shown. [Bounty](/systems/bounty/#display) describes who sees the text and for how long.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType
Bounty=yes
Bounty.Display=yes
```
