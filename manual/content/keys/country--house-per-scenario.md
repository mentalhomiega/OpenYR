---
key: Country
scope: house-per-scenario
label: House country
see_also: [ParentCountry, ActsLike, Color, PlayerControl]
when_omitted:
  kind: computed
  note: The house is the country its own name gives, as in Tiberian Sun. The name is a rules country, or a country of the map's `[Countries]`, or a new country with no settings of its own.
---

`Country` names the country a house plays. The value is the section name of a country from the rules or from the map's `[Countries]`, ignoring letter case. A house record that names one is a Yuri's Revenge house: it is a separate house, with the settings of that country as a starting point. Several houses can play the same country.

```ini title="scenario map file"
[Houses]
0=Allies
1=Soviets

[Soviets] ; a house record
Country=SovietsCountry
Credits=25
PlayerControl=yes
```

The house starts with the country's color, side, price and strength factors, and the house record's own [`Color=`](/keys/color/#scope-house-per-scenario) replaces the color. The house builds from the tech tree of the country it names, or, for a map country with [`ParentCountry=`](/keys/parentcountry/), of the rules country at the end of that chain. [`ActsLike=`](/keys/actslike/) in the same record replaces that tech tree.

A house record that names no `Country=` is a Tiberian Sun house, and the house's own name is its country. A house whose name is that of a rules country or a map country is that country's own house, whatever `Country=` says. A `Country=` that names no country is ignored and written to the debug log.

Everything in a map that names a house, such as an object's owner, a team's house, a trigger's house or [`Player=`](/keys/player/), uses the house name. A name that is a country with no house of that name finds the first house that plays it.
