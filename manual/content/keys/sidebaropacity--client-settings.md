---
key: SidebarOpacity
scope: client-settings
label: Sidebar opacity
see_also: [SidebarRows, InterfaceStyle]
when_omitted:
  kind: value
  value: "80"
  note: The panel's background is 80 percent solid, so the map makes up the other 20 percent of it.
---

`SidebarOpacity=` sets how solid the modern build panel's background is, as a whole number of percent from 0 to 100. It is written in `[Options]` of `RA2MD.INI`. The key applies only when [`InterfaceStyle`](/keys/interfacestyle/) is `Modern`, and the classic sidebar ignores it.

The background is the panel's dark fill and the wells behind the cameos, the readouts and the power bar. The map makes up 100 minus the value of each background colour. The radar, the buttons, the cameo art, the tab icons, the panel's border and all text stay opaque. A value of 100 draws the panel opaque, as it was before this key existed. A value of 0 leaves the background clear, so the map shows through all of it. Values below 0 act as 0, and values above 100 act as 100.

The **Sidebar opacity** slider is in the Interface section of the Graphics page, below **Sidebar rows**. It is greyed out while the **Modern in-game interface** switch is off. The value applies to the panel once OK is pressed, and Cancel keeps the saved value.
