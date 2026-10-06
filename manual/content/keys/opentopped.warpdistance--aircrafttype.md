---
key: OpenTopped.WarpDistance
scope: aircrafttype
label: 'Temporal hold distance in this transport'
see_also: [OpenToppedWarpDistance, OpenTopped, "system:temporal-weapons"]
when_omitted:
  kind: inherited
  note: The [CombatDamage] OpenToppedWarpDistance= value.
---

`OpenTopped.WarpDistance` sets how far, in cells, a [temporal](/systems/temporal-weapons/#letting-go) firer riding this [`OpenTopped=yes`](/keys/opentopped/) transport can hold its target before it lets go. It replaces the global [`OpenToppedWarpDistance`](/keys/opentoppedwarpdistance/) for this transport only. The key comes from Phobos.

```ini title="rulesmd.ini"
[BFRT] ; the stock Battle Fortress
OpenTopped.WarpDistance=8
```
