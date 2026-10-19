---
key: ToolTipColor
summary: The color of the border and text of the classic sidebar's tooltips for a side.
see_also: ["system:sidebar"]
when_omitted:
  kind: value
  value: none
  note: Tooltips are drawn in green.
---

Write the color as three numbers from 0 to 255 for red, green and blue. When the player's side sets the key, the classic interface draws a tooltip's border in that color and its text in a ramp of that color. The modern interface draws tooltips in its own panel style and ignores the key.

```ini title="rules.ini"
[FourthSide]
ToolTipColor=130,230,205
```

The key does not change the credits counter or any other text.
