---
key: LoadScreenText.SpecialName
summary: The string table label of the special unit's name printed on the country's loading screen.
see_also: ["system:loading-screens", "File.LoadScreen", "LoadScreenText.Brief"]
when_omitted:
  kind: computed
  note: The stock label for the country's position in `[Countries]`. A country with its own `File.LoadScreen` prints none.
---

Write a label from the string table. The text is printed in capitals, in black, at the stock place.
