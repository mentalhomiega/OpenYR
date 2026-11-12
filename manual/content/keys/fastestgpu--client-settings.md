---
key: FastestGpu
scope: client-settings
label: Fastest graphics card
when_omitted:
  kind: value
  value: "yes"
---

`FastestGpu=yes` draws the game on the graphics card that Windows ranks fastest. On a computer with two graphics cards, such as a laptop with one built into the processor and a separate one, the game otherwise draws on the first card Windows lists, which is often the slower one. The key is read from the `[Video]` section of `RA2MD.INI` at startup, and the game writes it back when it saves its settings. The Performance page of the settings sets it with the switch **Use the fastest graphics card**; a change applies at the next start.

With `FastestGpu=no` the game draws on the first card Windows lists. It does the same on a Windows version that does not rank its graphics cards, and on other systems.

When the game asks for a card, the debug log names it in a line that starts with `Video: asking for the graphics card`.
