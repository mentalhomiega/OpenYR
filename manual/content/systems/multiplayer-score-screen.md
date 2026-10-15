---
title: Multiplayer score screen
summary: "Ranks the houses that played a skirmish or network match and shows what each lost, killed, spent and scored."
category: multiplayer-networking
keys:
  - MultiplayPassive
  - SkipScore
related:
  - type: system
    id: observers
  - type: format
    id: spawn-ini
---

Skirmish and network matches end at this screen instead of the campaign score screen. Every player sees it when the match ends, whether they won or lost. A defeated player keeps watching the match while another human player is still undefeated, and sees the screen when the match ends. The match also ends once no human player is left undefeated, even while computer players fight on, so in a skirmish the player's defeat ends it at once. A recorded game being played back skips the screen.

Click, or press Space or Escape, to close the screen. If the launch file enables [multiplayer movies](/systems/multiplayer-movies/), the map's win or lose movie then plays. After that, a match started from the game's menu returns to the menu, and a match [a client launched](/formats/spawn-ini/) exits the game.

The title reads Skirmish Score in a skirmish and Multiplayer Score in a network match. The picture and the row bands come from the art of the player's side. The first side in the rules' `[Sides]` list uses the `MPA` art, the second uses `MPS`, and any other side uses `MPY`. At a render width of 640 the picture is the small one. The screen mounts `NTRLMD.MIX` and `NEUTRAL.MIX` while it is shown, because that is where Yuri's Revenge keeps the neutral art, and releases them when it closes.

## Who is listed

The screen has one row for each house that played. Houses whose country sets [`MultiplayPassive=yes`](/keys/multiplaypassive/) are left out, and so are [observers](/systems/observers/). Defeated players keep their rows, so a four-player match always ends with four rows.

Each row shows the house's name in the match, computer players included. With [`QuickMatch=yes`](/formats/spawn-ini/#what-a-player-is-shown) in the launch file, each row shows the player's number instead of a name.

Undefeated houses are listed first, then defeated ones. Within each group, the house with the higher score ranks first.

Each row's band is the side's bar art and does not take the house's color.

## What each column holds

| Column | What it counts |
| --- | --- |
| Kills | The vehicles, infantry, aircraft and structures the house destroyed or captured, whoever owned them, including the house itself. |
| Losses | The house's vehicles, infantry, aircraft and structures that were destroyed or captured. A sold structure does not count. |
| Built | The credits the house spent, as a percentage of what the biggest spender spent. The biggest spender reads 100. |
| Score | The points the house earned, plus a bonus if the house is undefeated. |

The columns appear in the order Player, Kills, Losses, Built and Score. The Built figure's meaning is not yet confirmed against Yuri's Revenge, which labels the column "Built".

A destroyed structure with [`Insignificant=yes`](/keys/insignificant/) counts in neither Losses nor Kills.

Destroying or capturing an object earns points equal to its cost. Harvesting also earns points. A kill that a super weapon credits to the house alone, such as a lightning strike, earns them too.

An undefeated house receives a bonus of half the average points of all the other listed houses, allies included. The bonus is at least 100.

A player who surrenders a skirmish is still ranked with the undefeated houses on this screen and receives the undefeated bonus.

Scores never increase down the list. If an undefeated house scored less than the house listed below it, its shown score becomes the lower house's score plus a random amount. The amount is between 1 and the average points of all listed houses, or between 1 and 100 if that average is below 100. A raised score can pass the house listed above, which is then raised the same way. Several undefeated houses, the winner included, can therefore show more than they earned.

The Kills, Losses and Built figures count up together. The Score figures count up after them. Each figure stops at its exact value.

A [saved game](/formats/save-games/) keeps the counts behind all four columns. A resumed match is therefore scored on the whole match, not only the part played since loading.

## Passing over the screen

[`SkipScoreScreen=yes`](/formats/spawn-ini/#what-a-player-is-shown) in the launch file ends a skirmish or network match without this screen. The ending movie still plays if the launch file enables multiplayer movies.

The map's [`SkipScore`](/keys/skipscore/) key applies only to the campaign score screen and has no effect here.
