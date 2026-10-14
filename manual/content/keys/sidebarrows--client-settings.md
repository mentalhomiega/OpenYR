---
key: SidebarRows
scope: client-settings
label: Sidebar rows
see_also: [InterfaceStyle]
when_omitted:
  kind: value
  value: "4"
  note: Four rows of three cameos, as OpenRA's sidebar shows. The panel holds the value to the rows the screen fits.
---

`SidebarRows=` sets how many rows of cameos the modern build panel shows at once. It is written in `[Options]` of `RA2MD.INI`. The key applies only when [`InterfaceStyle`](/keys/interfacestyle/) is `Modern`, and the classic sidebar ignores it.

Each row holds three cameos. The panel ends under its scroll arrows, so the map shows below it, and the power bar runs down the panel's left side to the same edge. The panel keeps the same number of rows on a taller screen, which leaves more map showing. Saved games do not store the value.

A value below 1 acts as 1. A value above the number of rows that fit between the top of the panel and its scroll arrows at the current screen height acts as that number.

Each scroll arrow under the cameos moves the shown tab by one screenful, which is as many rows as the panel shows. [`SidebarPageUp`](/commands/sidebarpageup/) and [`SidebarPageDown`](/commands/sidebarpagedown/) move it by a screenful too. [`SidebarUp`](/commands/sidebarup/) and [`SidebarDown`](/commands/sidebardown/) move it by one row.

The **Sidebar rows** drop-down on the Display page lists 1 up to the number of rows that fit at the current resolution. A small drawing beside the resolution list shows the picked rows, with the panel's tabs, rows and arrows. In a game, picking a value also resizes the real panel at once while the page is open. Cancel puts back the saved value. OK saves the picked value to `RA2MD.INI`.
