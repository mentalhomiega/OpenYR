---
key: DieSound.Override
scope: warheadtype
summary: "The sound played where this warhead destroys an object, in place of the object type's DieSound."
see_also: ["VoiceDie.Override", "UnitLost.Suppress", DieSound]
when_omitted:
  kind: value
  value: "none, so the type's DieSound= plays"
---

When this key is present, an object the warhead destroys plays the sound it names instead of the sounds of its type's [`DieSound=`](/keys/diesound/). `<none>` plays nothing. The sound plays at the object's place for every player who can hear it, as a type's death sound does.

```ini title="rulesmd.ini"
[MyQuietWH] ; example Warhead
DieSound.Override=<none>  ; a silent kill
```
