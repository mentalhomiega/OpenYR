---
title: Skirmish setup
summary: "Seats the person and up to seven computer players in a skirmish from a player list that gives each one a country, color, start position, team and difficulty."
category: multiplayer-networking
keys:
  - Handle
  - Color
  - Side
related:
  - type: system
    id: difficulty
  - type: system
    id: ui-files
  - type: format
    id: spawn-ini
---

The skirmish setup screen lists one row for each start position the chosen map has, up to eight. A map with fewer than two start positions still shows two rows. Choosing another map with the Multiplay Map button changes the number of rows.

The first row is the person at the keyboard: the name field, a country, a color, a start position and a team. Every other row starts with one of five choices:

| Choice | Effect |
| --- | --- |
| Open | The seat stays empty. |
| Closed | The seat stays empty. |
| Easy AI | A computer player at the easiest difficulty. |
| Medium AI | A computer player at the middle difficulty. |
| Hard AI | A computer player at the hardest difficulty. |

A row that holds nobody has its country, color, start and team boxes disabled. The game starts only with at least one computer player and with at least two players who are not all on one team; otherwise the screen says which is missing and stays open.

## Country, color and start

- The country box offers Random and the countries whose [`Multiplay=`](/keys/multiplay/) entry is on. A computer player given Random draws its country as it does in a game started from a launch file; the person's Random is settled when the game starts.
- The color box offers the eight lobby colors listed under [`Color`](/keys/color/). Colors never repeat: choosing one that another row holds hands that row the color this row had.
- The start box offers Random and the numbers of the map's start positions, as the map editor numbers its waypoints from 1. A numbered start is held by one row at a time, and choosing a start another playing row holds swaps the two. Rows set to Random draw from the positions nobody named.

## Teams

A team box offers None and teams 1 to 4. Players on the same team start the game allied with each other, in both directions. A player on no team is allied with nobody.

## Difficulty

Each computer player plays at the difficulty its row names. Hard AI gives the house the `[Easy]` section of the rules, Medium AI `[Normal]` and Easy AI `[Difficult]`, which are the [slots](/systems/difficulty/#from-the-setting-to-a-slot) a computer house reads at the Hard, Medium and Easy settings. The person plays at the middle slot.

## What the game remembers

Leaving the screen, whether to play or to cancel, writes each row to `[Skirmish]` in `RA2MD.INI` as `Slot1` to `Slot8`. Each value holds five numbers separated by commas: the choice for the seat (0 Open, 1 Closed, 2 Easy AI, 3 Medium AI, 4 Hard AI), the country's number in the rules' list or `-1` for Random, the color position, the start number or `0` for Random, and the team. The next visit reads them back. A value the screen cannot read, or one that names a country the rules no longer list, leaves that row at its default, which is Random for a country and a start.

The first row's name, country and color are the multiplayer preferences kept under [`Handle`](/keys/handle/), [`Side`](/keys/side/) and [`Color`](/keys/color/), which the network lobby shares. The game options on the screen (bases, crates, credits and the rest) are not written to `[Skirmish]`. Until a match starts from this screen, Fog Of War, Re-Deployable MCV, Multi Engineer, Superweapons, Build Off Ally, Short Game and Game Speed show the values of the `[MultiplayerDialogSettings]` section in the rules, as [multiplayer rules](/formats/multiplayer-rules/) describes. After a match starts from it, these options keep that match's values for the rest of the run.

A game started from a [launch file](/formats/spawn-ini/) does not use this screen or `[Skirmish]`.
