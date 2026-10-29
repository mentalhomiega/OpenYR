---
key: VoiceSelect
summary: The responses an object speaks when it is selected.
see_also: [VoiceMove, VoiceAttack, VoiceDie, VoiceFeedback, VoiceComment, Selectable]
when_omitted:
  kind: value
  value: ""
---

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
VoiceSelect=MYTANK_Sel1,MYTANK_Sel2 ; sound IDs registered in SOUND.INI
```

When the player selects an object of this type, it speaks one entry picked at random from the list. The response is a sound without a place, so it does not fade or pan with the object's position on screen; see [Placed sounds](/systems/sound-effects/#placed-sounds). Only objects of a house the player controls speak.

Selecting several objects at once with a band box, a group key or a command that picks many objects produces one response, from the first object selected. The [`TypeSelect`](/commands/typeselect/) and [`SelectType`](/commands/selecttype/) commands select quietly and produce none. A press of [`VeterancyFilter`](/commands/veterancyfilter/) or [`HealthFilter`](/commands/healthfilter/), or of a filter's add-lower form, produces one response, from the first object it keeps or adds.

An object says one response at a time. A response chosen while the object is still speaking waits until it has finished, replaces any other response that is waiting, and is dropped if it is the sound already playing. Different objects speak at the same time.

## Writing the list

This section also applies to [`VoiceMove`](/keys/voicemove/), [`VoiceAttack`](/keys/voiceattack/), [`VoiceDie`](/keys/voicedie/) and [`VoiceFeedback`](/keys/voicefeedback/).

Each name is matched, without regard to letter case, against the sound IDs registered in [SOUND.INI](/formats/sound-ini/). A name that matches no sound is dropped. Do not put spaces after the commas: a name with a leading space matches nothing and is dropped.

A later rules file that sets the key replaces the earlier list. Writing the key with nothing after the `=` does not clear it, because the game ignores an empty assignment. To silence a type that an earlier file gave a list, write a value that matches no sound, such as `<none>`.
