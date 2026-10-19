---
key: LoadingTheme
scope: housetype
label: Country loading theme
see_also: ["system:loading-screens"]
when_omitted:
  kind: computed
  note: The loading theme of the country's side.
---

The value names a theme from the theme file, such as `FoehnLoad`. A skirmish or network match plays it for the local player while the match loads, from the point where the rules have been read until the map is ready. A name that the theme file lacks plays nothing and is reported in the debug log. A player who watches hears no loading theme.

```ini title="rules.ini"
[Guild1]
LoadingTheme=FoehnLoad
```
