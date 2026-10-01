---
key: OccupyPip
summary: The figure a garrisoned structure shows for this soldier in its occupant row.
see_also: [Occupier, ShowOccupantPips, Pip, "system:garrisons"]
when_omitted:
  kind: context-dependent
  note: PersonWhite for a type whose section appears in one rules file. Each further rules file that has the section without `OccupyPip` changes the figure again.
---

A garrisoned structure that is selected or under the mouse shows one figure per occupant slot. A slot this soldier fills shows the figure named here. Name a [pip color](/reference/enums/pip-color/) from the `person` group, such as `PersonBlue`, in any letter case.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Occupier=yes
OccupyPip=PersonBlue
```

:::caution[Set `OccupyPip` in every file that has the section]
A rules file that has this soldier's section but no `OccupyPip` does not keep the figure set earlier; it replaces it as [Omitted pip colors](/reference/enums/pip-color/#omitted-pip-colors) shows. A soldier type whose section appears in only one rules file, without the key, therefore shows `PersonWhite`. A value that matches no pip color sets `green`, which draws a plain green pip in place of a figure.
:::
