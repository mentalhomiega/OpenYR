---
key: File.Flag
summary: The PCX file shown as the country's flag beside a player's name on the loading screen.
see_also: ["system:loading-screens", "File.LoadScreen"]
when_omitted:
  kind: computed
  note: The stock flag for the country's position in `[Countries]`, and no flag for a later position.
---

Write the file name with its `.PCX` extension. Magenta pixels are transparent. Each row of the loading screen is laid out for the size of the first stock flag, whatever flag it shows.
