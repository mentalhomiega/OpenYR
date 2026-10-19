---
key: EVA.Lost
summary: The EVA line announced when the player loses an object of this type, or "none" for silence.
see_also: [Insignificant, DontScore, "system:eva-speech", LostEvaEvent]
when_omitted:
  kind: computed
  note: A unit announces EVA_UnitLost; a structure announces its own stock line when an enemy destroys it.
---

`EVA.Lost` names the line of [EVAMD.INI](/formats/eva-ini/) that the player hears when an object of this type that the player controls is destroyed. `none` announces nothing. Setting the key replaces the stock announcement.

The line is not spoken for a type with [`Insignificant=yes`](/keys/insignificant/), `Spawned=yes` or [`DontScore=yes`](/keys/dontscore/). A unit's line also waits for the radar event of its loss, which the game raises once in a while, so a second loss soon after another announces nothing. A structure with the key announces its loss whoever destroyed it; without the key, only a structure that something attacked announces it.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType
EVA.Lost=none
```
