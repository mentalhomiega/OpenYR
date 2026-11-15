---
key: VoiceAirstrikeAttack
summary: "The sound played when the player calls an airstrike that this designator or aircraft type takes part in."
see_also: [AirstrikeAttackVoice, Airstrike]
when_omitted:
  kind: inherited
  note: "The aircraft type's value, then [AudioVisual] AirstrikeAttackVoice."
---

`VoiceAirstrikeAttack` is an Ares key. When one of the player's objects calls an airstrike with an [`Airstrike`](/keys/airstrike/) weapon, the designator's `VoiceAirstrikeAttack` plays in place of [`AirstrikeAttackVoice`](/keys/airstrikeattackvoice/). A designator type without the key uses the value on the type of the aircraft it calls, and with neither the `[AudioVisual]` sound plays.

```ini title="rulesmd.ini"
[MYSPOTTER] ; example InfantryType
VoiceAirstrikeAttack=MIGMove
```
