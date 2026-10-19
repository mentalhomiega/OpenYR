---
key: VoiceDie.Override
scope: warheadtype
summary: "The voice played where this warhead destroys an object, in place of the object type's VoiceDie."
see_also: ["DieSound.Override", "UnitLost.Suppress", VoiceDie]
when_omitted:
  kind: value
  value: "none, so the type's VoiceDie= plays"
---

When this key is present, an object the warhead destroys plays the voice it names instead of the voices of its type's [`VoiceDie=`](/keys/voicedie/). `<none>` plays nothing. Only the player who owns the destroyed object hears the voice, as with a type's death voice.

```ini title="rulesmd.ini"
[MyQuietWH] ; example Warhead
VoiceDie.Override=<none>
```
