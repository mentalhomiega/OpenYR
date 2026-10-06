---
key: Player
scope: scenarios
label: Commanded house
see_also: [SpeechSide, NextScenario]
when_omitted:
  kind: computed
  note: The first house whose record sets `PlayerControl=yes`, or the first house in `[Houses]` when none does, the same as a value that names no house.
---

```ini title="map file"
[Basic]
Player=GDI
```

`Player` names the house the player controls in a campaign mission. The name is a house name from `[Houses]`, ignoring case. A country's section name or `Name=` also works when it names a house of the mission, and then picks the first house that plays that country. The player commands that house's units and structures, spends its credits, and builds from its build options. The map selection screen after the mission reads its campaign progression from the same house.

In a multiplayer or skirmish game, the lobby or the launch file assigns houses and this key is ignored.

:::caution[Name a house the mission contains]
A value that names no house of the mission leaves the player with the first house whose record sets [`PlayerControl=yes`](/keys/playercontrol/), or with the first house in `[Houses]` when none does. The game notes the substitution in its debug log.
:::
