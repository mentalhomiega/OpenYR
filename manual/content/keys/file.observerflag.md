---
key: File.ObserverFlag
summary: The flag file Ares shows for the country in observer mode.
see_also: ["File.ObserverBackground", "File.ObserverFlagAltPalette"]
when_omitted:
  kind: value
  value: none
---

The value is a SHP or PCX file name with its extension. The game reads the key and draws nothing from it, because the observer panel that shows the flag is not in this build.

```ini title="rules.ini"
[UnitedStates]
File.ObserverFlag=unst.pcx
```
