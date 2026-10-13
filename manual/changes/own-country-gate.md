---
title: Check RequiredHouses against the house's own country
category: fix
release: 0.2.0
targets:
- type: key
  id: RequiredHouses
  effect: changed
- type: key
  id: ForbiddenHouses
  effect: changed
- type: key
  id: ActsLike
  effect: changed
credit: [MentalHomiega]
---

A house now tests `RequiredHouses=` and `ForbiddenHouses=` against its own country, the one its map house record names with `Country=`. Before, the test used the country `ActsLike=` names, or the country at the end of a `ParentCountry=` chain, so a house based on Americans could not build `DOG`, which forbids Americans, and could build a type that requires Americans although its own country is not listed. `ActsLike=` still decides `Owner=` and what computer base planning builds.

Saved games made by earlier builds are refused, because the house's own country is stored.
