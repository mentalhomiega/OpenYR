---
key: EVA.VeteranPromoted
summary: The EVA line announced when the player's object of this type is promoted to veteran.
see_also: ["EVA.ElitePromoted", "Promote.VeteranSound", "Promote.EliteSound", "system:veterancy", "system:eva-speech"]
when_omitted:
  kind: value
  value: "EVA_UnitPromoted"
---

`EVA.VeteranPromoted` names the line of [EVAMD.INI](/formats/eva-ini/) that the player hears when an object of this type that the player controls reaches veteran rank. `none` announces nothing, and the promotion sound still plays. Without the key, or with an empty value, the line is `EVA_UnitPromoted`.

An object created at a rank is not announced, and an object another player controls is not announced to the player.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
EVA.VeteranPromoted=EVA_DefenseUpgraded
```
