---
key: ChuteSound
summary: Sound of a parachute opening under a passenger dropped from an aircraft.
see_also: [Parachute, Passengers, "system:drop-pods"]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[AudioVisual]
ChuteSound=CHUTE1 ; placeholder for your sound's ID from SOUND.INI
```

`ChuteSound` plays at an aircraft's position each time it drops a passenger by parachute, so a full load plays the sound once per passenger. A [paradrop](/systems/superweapons/#paradrops) plane drops its paratroopers over its target, and any other aircraft carrying passengers drops one each time it would fire its weapon. The sound plays only after the passenger has been placed. If the passenger cannot be placed, it goes back aboard and no sound plays.

A [drop pod](/systems/drop-pods/) is a separate kind of descent and plays the sounds of [`DropPodWeapon`](/keys/droppodweapon/) instead.
