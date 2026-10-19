---
key: Sonar.Duration
summary: The frames an object hit by the warhead is kept from cloaking.
see_also: [CloakDelay, EffectsRequireDamage, EffectsRequireVerses, "SonarPulse.Delay", "system:cloaking"]
when_omitted:
  kind: value
  value: "0, which has no effect"
---

A positive value decloaks a cloaked or cloaking object that the warhead hits, and keeps it from cloaking again for that many game frames. An object that is not cloaked is kept from cloaking the same way. The effect works as a sonar pulse does: the object cannot cloak while the delay runs, whatever [`CloakDelay`](/keys/cloakdelay/) says.

The delay never shortens one already running, so a hit during the normal delay after uncloaking extends it to this value, and a second hit with a shorter value changes nothing. The hit must reach an object whose armor has a nonzero [`Verses`](/keys/verses/) for the warhead, or any armor with [`EffectsRequireVerses=no`](/keys/effectsrequireverses/), and, with [`EffectsRequireDamage=yes`](/keys/effectsrequiredamage/), it must damage the object. A hit that heals does nothing, and neither does an unforced hit on an object under the Iron Curtain.

```ini title="rulesmd.ini"
[SonarWH] ; example WarheadType
Sonar.Duration=300
```
