---
enum_id: PipEnum
slug: pip-color
title: Pip color
summary: Named pip colors accepted for passenger pips and garrison figures.
representation: token
bindings:
  key_value_types: [pipenum]
  scripting_parameter_types: []
source_files: [code/pip.hh, code/ccini.cpp]
values:
  - { constant: PIP_GREEN, value: 1, input: "green", meaning: "Green filled pip." }
  - { constant: PIP_YELLOW, value: 2, input: "yellow", meaning: "Yellow filled pip." }
  - { constant: PIP_WHITE, value: 3, input: "white", meaning: "White filled pip." }
  - { constant: PIP_RED, value: 4, input: "red", meaning: "Red filled pip." }
  - { constant: PIP_BLUE, value: 5, input: "blue", meaning: "Blue filled pip." }
  - { constant: PIP_PERSON_GREEN, value: 7, input: "persongreen", meaning: "Green figure, for a garrison occupant." }
  - { constant: PIP_PERSON_YELLOW, value: 8, input: "personyellow", meaning: "Yellow figure, for a garrison occupant." }
  - { constant: PIP_PERSON_WHITE, value: 9, input: "personwhite", meaning: "White figure, for a garrison occupant." }
  - { constant: PIP_PERSON_RED, value: 10, input: "personred", meaning: "Red figure, for a garrison occupant." }
  - { constant: PIP_PERSON_BLUE, value: 11, input: "personblue", meaning: "Blue figure, for a garrison occupant." }
  - { constant: PIP_PERSON_PURPLE, value: 12, input: "personpurple", meaning: "Purple figure, for a garrison occupant." }
---

A **pip** is one of the small markers drawn in a row beneath a selected object. These names are the values [`Pip`](/keys/pip/) and [`OccupyPip`](/keys/occupypip/) accept, in any letter case. `Pip` colors the pips a transport draws for an infantry passenger, and `OccupyPip` picks the figure a garrisoned structure draws for an occupant. Either key accepts any name, but the plain colors are pips and the `person` names are figures.

A name that matches none of these sets `green`. The empty pip and the empty figure that mark free space have no name, and neither do the veteran and elite insignia.

## Omitted pip colors

A rules file that has a soldier's section but leaves out `Pip` or `OccupyPip` does not keep the value held before it. The key takes the value in the right-hand column below, as Yuri's Revenge does. The first file to read the section starts from `green` for `Pip` and from `persongreen` for `OccupyPip`, so a soldier type whose section is in only one rules file gets `yellow` and `personwhite`.

| Held before the file | Held after it |
| --- | --- |
| `green` | `yellow` |
| `yellow` | `white` |
| `white` | `red` |
| `red` | `blue` |
| `blue` | `persongreen` |
| `persongreen` | `personwhite` |
| `personyellow` | `personred` |
| `personwhite` | `personblue` |
| `personred` | `personpurple` |
| `personblue` or `personpurple` | `green` |

A file that does not have the soldier's section changes neither key.
