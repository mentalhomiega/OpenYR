---
key: LoadScreenText.Color
summary: The color scheme of the text on the country's loading screen.
see_also: ["system:loading-screens", "File.LoadScreen"]
when_omitted:
  kind: computed
  note: '`AlliedLoad` for a country on the first side in `[Sides]` and `SovietLoad` for every other country.'
---

Name a scheme from `[Colors]`. A name that matches no scheme leaves the default. The color applies to the country name, the description and the word Loading, not to the player rows.
