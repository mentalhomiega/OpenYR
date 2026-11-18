---
title: Add StockArtFixes for drawing broken stock art as intended
category: feature
release: 0.2.0
targets:
- type: key
  id: StockArtFixes
  effect: added
credit:
- MentalHomiega
---

[`StockArtFixes=no`](/keys/stockartfixes/) draws stock art the way the original game does. It is on by default, so the city apartment blocks `CITY01` to `CITY05` draw through the city palette as smooth walls and roofs, where the original shows speckled noise. A mod that sets `Palette=` for a block keeps its own value.
