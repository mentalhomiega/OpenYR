---
key: CustomColor
scope: multiplayer-settings
label: Preferred multiplayer free color
see_also: ["Color", "Handle"]
when_omitted:
  kind: value
  value: "0"
---

The value is a free color packed into one number: bit 24 marks it as a free color, bits 16 to 23 hold the hue, bits 8 to 15 the saturation and bits 0 to 7 the value. The game writes it when you leave the lobby with a free color picked. A value without bit 24 is ignored.

Zero, the default, means no free color. The player then draws in the stock color at [`Color`](/keys/color/#scope-multiplayer-settings). A value whose brightness is below 96 is raised to 96 when it is read.
