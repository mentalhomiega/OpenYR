---
key: File.ObserverBackground
summary: The 121 by 96 picture Ares shows behind the country's flag in observer mode.
see_also: ["File.ObserverFlag", "File.ObserverFlagAltPalette"]
when_omitted:
  kind: value
  value: none
---

The value is a SHP or PCX file name with its extension. The game reads the key and draws nothing from it, because the observer panel that shows the background is not in this build.

```ini title="rules.ini"
[UnitedStates]
File.ObserverBackground=obsalli.pcx
```
