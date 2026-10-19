---
key: IronCurtain.Duration
scope: warheadtype
summary: "Puts the objects a warhead hits under the Iron Curtain for this many frames, or shortens their curtain when negative."
see_also: ["IronCurtain.Cap", "IronCurtain.Modifier", IronCurtainDuration, EffectsRequireVerses]
when_omitted:
  kind: value
  value: "0, so the warhead puts nothing under the Iron Curtain"
---

A warhead with a positive `IronCurtain.Duration` puts each object its blast damages and whose armor has a nonzero `Verses` against it under the Iron Curtain. A negative value takes frames off a curtain the object is under, and `-99999` ends it. [`IronCurtain.Cap=`](/keys/ironcurtain.cap/) sets how the value joins a curtain that is already running. [`EffectsRequireVerses=no`](/keys/effectsrequireverses/) also lets it reach a 0% armor, and [`EffectsRequireDamage=yes`](/keys/effectsrequiredamage/) holds it back from a hit that deals no damage.

The curtain starts after the hit's own damage lands, so an object the hit destroys gets none. An object already under the curtain takes no damage and still receives the new duration. A type's [`IronCurtain.Modifier=`](/keys/ironcurtain.modifier/) multiplies a positive duration, and a type that sets it to `0` is not affected. A soldier or an `Organic=yes` vehicle that receives the curtain is destroyed instead, as it is when a super weapon curtains it.

```ini title="rulesmd.ini"
[MyGuardWH] ; example Warhead
IronCurtain.Duration=300  ; twenty seconds
```
