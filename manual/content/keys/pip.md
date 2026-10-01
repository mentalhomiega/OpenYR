---
key: Pip
summary: The color of the pip a transport draws for this soldier while it is carried.
see_also: [Passengers, PipScale, MaxPips, Size, OccupyPip, "system:transports"]
when_omitted:
  kind: context-dependent
  note: yellow for a type whose section appears in one rules file. Each further rules file that has the section without `Pip` changes the color again.
---

`Pip` sets the color of the pips this soldier fills in a transport's pip row while it rides inside. The row appears under a selected transport that the player or an ally owns, with the exceptions [`PipScale`](/keys/pipscale/) lists. Each passenger fills as many pips as its [`Size`](/keys/size/#scope-aircrafttype), and at least one. An infantry passenger's pips take its type's `Pip` color, any other passenger's pips are green, and free space is drawn empty. The value has no other effect, so it does nothing for an infantry type that never rides in a transport.

```ini title="rulesmd.ini"
[MYCOMMANDO] ; example InfantryType
Pip=white
```

The row's length comes from the transport's [`PipScale`](/keys/pipscale/) and [`MaxPips`](/keys/maxpips/). Under `PipScale=Passengers` the row has five pips, or `MaxPips` where set, but never more than the transport's [`Passengers`](/keys/passengers/). Passengers past the end of the row get no pip.

:::caution[Set `Pip` in every file that has the section]
A rules file that has this soldier's section but no `Pip` does not keep the color set earlier; it replaces it as [Omitted pip colors](/reference/enums/pip-color/#omitted-pip-colors) shows. A value that matches none of the [pip colors](/reference/enums/pip-color/) sets `green`, so a misspelling in a later file also undoes an earlier assignment.
:::
