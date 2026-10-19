---
key: LoadScreenText.Brief
summary: The string table label of the description printed on the country's loading screen.
see_also: ["system:loading-screens", "File.LoadScreen", "LoadScreenText.SpecialName"]
when_omitted:
  kind: computed
  note: The stock label for the country's position in `[Countries]`. A country with its own `File.LoadScreen` prints none.
---

Write a label from the string table. The text wraps inside the stock description box and is printed in the country's loading screen color.
