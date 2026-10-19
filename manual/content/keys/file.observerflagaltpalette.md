---
key: File.ObserverFlagAltPalette
summary: Chooses the palette Ares draws a SHP observer flag with.
see_also: ["File.ObserverFlag", "File.ObserverBackground"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a SHP named by `File.ObserverFlag=` is drawn with `yrii.pal`; otherwise it is drawn with `observer.pal`. The game reads the key and draws nothing from it, because the observer panel that shows the flag is not in this build.
