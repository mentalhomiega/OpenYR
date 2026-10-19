---
key: LoadingTheme
scope: side
label: Side loading theme
see_also: ["system:loading-screens"]
when_omitted:
  kind: value
  value: none
---

The value names a theme from the theme file. A country that sets no [`LoadingTheme=`](/keys/loadingtheme/#scope-housetype) plays its side's theme while a skirmish or network match loads. A side that sets none leaves the music as it was, where Ares plays the `LOADING` theme.

```ini title="rules.ini"
[FourthSide]
LoadingTheme=FoehnLoad
```
