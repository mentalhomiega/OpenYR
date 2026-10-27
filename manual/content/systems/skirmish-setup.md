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

A row that holds nobody has its country, color, start and team boxes disabled. A game with no computer player starts with the person alone, whatever the person's team. A game with computer players starts only when its players are not all on one team; otherwise the screen says so and stays open. A map with fewer start positions than the game needs shows a message and keeps the screen open.

## Country, color and start

- The country box offers Random and the countries whose [`Multiplay=`](/keys/multiplay/) entry is on. A computer player given Random draws its country as it does in a game started from a launch file; the person's Random is settled when the game starts.
- The color box offers the eight lobby colors listed under [`Color`](/keys/color/). Colors never repeat: choosing one that another row holds hands that row the color this row had.
- The start box offers Random and the numbers of the map's start positions, as the map editor numbers its waypoints from 1. A numbered start is held by one row at a time, and choosing a start another playing row holds swaps the two. Rows set to Random draw from the positions nobody named.

## Teams

A team box offers None and teams 1 to 4. Players on the same team start the game allied with each other, in both directions. A player on no team is allied with nobody.

## A game with no computer player

With every row after the first set to Open or Closed, the person plays alone. No opponent existing does not end the game. It ends when the person's house is defeated, when the person quits, or when a trigger on the map ends it. In such a game the person's team has no effect.

## Difficulty

Each computer player plays at the difficulty its row names. Hard AI gives the house the `[Easy]` section of the rules, Medium AI `[Normal]` and Easy AI `[Difficult]`, which are the [slots](/systems/difficulty/#from-the-setting-to-a-slot) a computer house reads at the Hard, Medium and Easy settings. The person plays at the middle slot.

## What the game remembers

Leaving the screen, whether to play or to cancel, writes the setup to `[Skirmish]` in `RA2MD.INI`:

- `Map` holds the file name of the chosen map.
- `Slot1` to `Slot8` hold the rows. Each value holds five numbers separated by commas: the choice for the seat (0 Open, 1 Closed, 2 Easy AI, 3 Medium AI, 4 Hard AI), the country's number in the rules' list or `-1` for Random, the color position, the start number or `0` for Random, and the team.
- `Units`, `Credits`, `Tech` and `Speed` hold the unit count, the credits, the tech level and the game speed. `Bases`, `Crates`, `Fog`, `Bridges`, `Redeploy`, `ShortGame`, `Engineer`, `SuperWeapons` and `BuildOffAlly` hold the switches as `0` or `1`.

The screen reads these keys each time it opens. The setup therefore comes back after a game started from the screen ends, after a cancel, and after the game is closed and started again. A switch or number the file lacks leaves that setting at the rules' value.

The remembered map is used only while its file is on hand; otherwise the first map that is on hand takes its place. A map with fewer start positions than the saved players keeps the rows it can seat, and the rows past its last start position stay in the file. A country or start that the lists no longer hold becomes Random. A kind past the end of the list is held at the last kind.

Until the screen has written its options, Fog Of War, Re-Deployable MCV, Multi Engineer, Superweapons, Build Off Ally, Short Game and Game Speed show the values of the `[MultiplayerDialogSettings]` section in the rules, as [multiplayer rules](/formats/multiplayer-rules/) describes.

The first row's name, country and color are the multiplayer preferences kept under [`Handle`](/keys/handle/), [`Side`](/keys/side/) and [`Color`](/keys/color/), which the network lobby shares.

A game started from a [launch file](/formats/spawn-ini/) does not use this screen or `[Skirmish]`.
