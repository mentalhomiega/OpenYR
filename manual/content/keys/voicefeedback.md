---
key: VoiceFeedback
summary: The responses an object speaks on the hit that takes it below half strength.
see_also: [VoiceDie, VoiceSelect, VoiceMove, VoiceAttack, DamageParticleSystems]
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
VoiceFeedback=MYTANK_Hurt1 ; a sound ID registered in SOUND.INI
```

An object can speak this list on the hit that takes it from half its maximum strength or more to below half. The hit speaks only if **all of** these hold:

1. It does not destroy the object.
2. It does not also take the object below the [`ConditionRed`](/keys/conditionred/) threshold. A hit that crosses both thresholds at once plays no response.
3. A 30 percent chance succeeds.
4. The local player controls the object. An enemy object stays silent, even when the chance succeeds.

Further hits while the object stays below half strength play nothing. Once the object is repaired back to half strength or more, the next hit that crosses the threshold can speak again.

One entry is picked at random and played as a placed sound at the object's position, so it fades and pans with where the object is on screen; see [Placed sounds](/systems/sound-effects/#placed-sounds).

Names are matched as described in [Writing the list](/keys/voiceselect/#writing-the-list).
