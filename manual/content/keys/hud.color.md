---
key: Hud.Color
summary: The color the modern in-game interface frames its panels and accents in for a side.
see_also: ["system:sidebar", InterfaceStyle]
when_omitted:
  kind: computed
  note: The three stock sides keep their blue, red and purple. Any other side takes the hue of the color scheme of its first country, and a side whose first country has no colored scheme keeps the Allied blue.
---

Write the color as three numbers from 0 to 255 for red, green and blue. When the player's side sets the key, the [modern interface](/keys/interfacestyle/) draws the panel and command bar frames, the active tab's outline and the pressed accents in that color, and turns the hue of the Allied button icons to match it. A color close to grey leaves the icons blue.

```ini title="rules.ini"
[FourthSide]
Hud.Color=60,200,190
```

A side that omits the key takes its color from the hue of the scheme named by the `Color=` key of its first country in `[Countries]`, at the brightness the stock colors have. The classic interface ignores the key. The tabs, buttons and commands keep their glyphs and positions whatever the color, so the panel stays readable without telling colors apart.
