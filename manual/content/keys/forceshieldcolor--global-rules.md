---
key: ForceShieldColor
scope: global-rules
label: Force Shield tint
summary: The ColorAdd entry that tints a structure while a Force Shield protects it.
see_also: [ForceShieldDuration, ForceShieldRadius]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rulesmd.ini"
[AudioVisual]
ForceShieldColor=6
```

The value is a position in the `[ColorAdd]` list. A structure under a Force Shield is drawn with that color added, and so are its animations. The tint goes when the shield wears off.

Entry `0` adds nothing, so the structure keeps its normal colors. A structure drawn in the terrain palette takes no tint. A structure that is also marked by an airstrike shows both colors combined.
