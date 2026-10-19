---
key: LoadScreenText.Name
summary: The string table label of the country name printed on its loading screen.
see_also: ["system:loading-screens", "File.LoadScreen"]
when_omitted:
  kind: computed
  note: The stock label for the country's position in `[Countries]`. A country with its own `File.LoadScreen` prints no name.
---

Write a label from the string table, such as `Name:Americans`. The name is printed right-aligned at the stock place, in the country's loading screen color.
