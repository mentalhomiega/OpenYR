---
key: SkipMapSelect
summary: Whether the campaign advances straight to a named mission instead of taking the stage's first choice.
see_also: [NextScenario, AltNextScenario, OneTimeOnly, EndOfGame]
when_omitted:
  kind: value
  value: "no"
---

```ini title="map file"
[Basic]
SkipMapSelect=yes
NextScenario=Maps/Missions/GDI2A.MAP
AltNextScenario=Maps/Missions/GDI9C.MAP
```

With `SkipMapSelect=yes`, the mission names its successor. The campaign advances to [`NextScenario`](/keys/nextscenario/), or to [`AltNextScenario`](/keys/altnextscenario/) when global variable 1 is set. At the default, the campaign advances to the first choice of the mission's stage, and the two names are not used.

The named map must be one of the next missions the campaign's map selection data offers from the current stage, and the campaign moves to that stage. [`NextScenario`](/keys/nextscenario/) covers what happens when the name is not among them.

A mission that also sets [`OneTimeOnly`](/keys/onetimeonly/) or [`EndOfGame`](/keys/endofgame/) ends the campaign instead, and this key has no effect.
