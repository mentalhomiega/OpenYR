---
title: Observers and coach mode
summary: "Observers who watch a skirmish or network game without playing, what a player given the whole map sees, and what coach mode lets a defeated player keep."
category: multiplayer-networking
keys:
  - MultiplayPassive
  - ShroudGrow
  - ShroudRate
  - FogRate
  - CreditTicks
related:
  - type: format
    id: spawn-ini
  - type: format
    id: save-games
  - type: system
    id: map-visibility
  - type: system
    id: cloaking
  - type: system
    id: multiplayer-score-screen
  - type: system
    id: sidebar
  - type: system
    id: veterancy
  - type: system
    id: power
  - type: system
    id: ion-storms
---

A client seats an observer by listing the seat under `[IsSpectator]` in [the launch file](/formats/spawn-ini/#who-is-playing). The observer watches from the first frame and never plays. The menu lobbies cannot seat an observer.

Each observer still gets a house. The house uses the country the launch file gives the seat, so the sidebar art and the EVA voice follow that country. It starts defeated and receives no start position, construction vehicle or starting units. The observer's view opens on the start position of a randomly chosen playing house, or on the center of the map when no house has one.

An observer's house holds no alliances. The launch file's alliance entries for the seat are ignored, and so are other seats' entries that name it. No house can ally with it during play, and it cannot ally with another house.

Because the house starts defeated, it never counts among the houses or people still playing when the game decides whether the match is over. It cannot surrender.

An observer does not appear on the [score screen](/systems/multiplayer-score-screen/), in the radar pane's name and kill list, or in the statistics report.

An observer's house is still an observer after a [saved game](/formats/save-games/) is loaded. A launch file that resumes a network game's save must mark the same seats as watching, or the game refuses to load the save.

An observer owns nothing, and a defeated player's objects are destroyed when the defeat is accepted. The [Destroy all of...](/mapping/actions/taction-house-destroy-all/) action describes what that takes. Mind control is the exception. A unit the defeated player holds keeps the defeated player as its owner, and a unit another house holds from the defeated player survives under that house.

## The whole map

An observer is given the whole map, and so is a defeated player in a match without [coach mode](#coach-mode). Both see the match the same way:

| Subject | What is shown |
| --- | --- |
| Shroud and fog | Every cell is revealed and unfogged, and the fogged stand-ins for structures are discarded. Neither [regrowth pass](/systems/map-visibility/#losing-ground-again) covers the map again, and neither does [Reshroud map](/mapping/actions/taction-reshroud/) or its [team mission](/mapping/missions/tmission-reshroud/). |
| Radar | The pane stays up whatever the [power](/systems/power/#radar) and [ion storm](/systems/ion-storms/#radar) tests say, and it shows the whole map. |
| Hidden objects | A cloaked object is drawn shadowy and can be selected. Cloaked and underground objects are plotted on the radar. None of them raises a [detection](/systems/cloaking/#on-the-radar) event. |
| Decorations | Every house's objects show cargo pips, [rank insignia](/systems/veterancy/#rank-display) and the healer's cross. A selected structure that generates power shows its house's power output and drain. A selected factory shows the cameo of what it is building only when the observer's country has spied on it ([infiltrating it](/systems/capture/#infiltrating-it)). A tooltip names an object by its type, not as an enemy. |
| Disguises | They hold, as they do for every player except the owner. |
| Chat | Messages can go to everyone. An observer can also message the other observers. A team or private message cannot be opened ([in-game chat](/systems/chat/)). |

Clicking an object of a defeated player therefore only selects it for viewing and gives no order.

## Commands

A watcher cannot use the `PlaceBeacon`, `NextObject`, `PreviousObject`, `CombatantSelect`, `HealthFilter`, `VeterancyFilter`, `TypeSelect` and `AllToCheer` commands, and planning mode does not start. In a network match, a watcher's game speed change is not sent to the other players. Coach mode leaves these to a defeated player.

## What an observer hears

EVA speaks most lines only to the house they concern: money, silos, power, a lost unit or structure, a harvester or base under attack, a superweapon, the sidebar, and the house's own defeat or victory. An observer owns nothing and is never given a win, so it hears none of these lines. When the match ends for it, the loss caption appears without the defeat voice line.

Lines spoken to everyone still reach an observer. These include another player's defeat, a missile launch, an approaching ion storm, and anything a scenario scripts. The text messages that announce a defeat or a departure appear as they do for every player.

Radar events follow the same rule: the only ones an observer sees are those a scenario scripts.

## The sidebar and the credit readout

The [sidebar](/systems/sidebar/) stays up, with nothing on either strip. Its credit readout shows how long the match has run instead of money. The time counts real seconds while the game is not paused, and reads `Time:MM:SS`, or `Time:HH:MM:SS` once the match has run an hour. The readout plays no [`CreditTicks`](/keys/creditticks/) sound.

## Coach mode

`CoachMode=yes` in the launch file's `[Settings]` section decides what a defeated player keeps. Only the launch file can turn it on; the menu lobbies have no such option.

Without coach mode, a defeated player is given [the whole map](#the-whole-map), as an observer is, and can only send messages to everyone.

With coach mode, defeat leaves the player's view and chat as they were:

- Nothing is revealed at defeat. The fog stays drawn, the regrowth passes keep running, and the player goes on seeing what their allies reveal under [`AllyReveal=yes`](/keys/allyreveal/).
- The radar goes down with the player's radar structure, unless the map gives everyone radar. An [ion storm](/systems/ion-storms/#radar) still takes it down.
- Hidden enemy objects and enemy decorations stay hidden.
- Team and private messages stay open, so the player can keep talking to their allies.

With or without coach mode, a defeated player keeps their row on the score screen, in the radar pane's name list and in the statistics report.

## When the match ends

The game checks whether the match is over each time a house is defeated. A scripted [Destroy all of...](/mapping/actions/taction-house-destroy-all/) action runs the same check. A house whose country sets [`MultiplayPassive=yes`](/keys/multiplaypassive/), such as `Neutral`, is not counted.

After a defeat, the match ends when either of these holds for the houses still alive:

- They are one side, meaning every pair of them is allied in both directions.
- No person is among them. An observer's house starts defeated, so it never counts.

Otherwise the match goes on: some two houses still alive are not allied in both directions, and a person is still alive. When the match ends, each person's house wins if it is still alive and loses if it was defeated.

A match in which every person watches has no person alive from the start, so the first defeat of any house ends it. A match whose houses all start on one side ends at the first defeat, because its survivors are one side.

## Seating

Any number of people may watch. The game refuses a launch file in which every person watches and no computer plays. In a match where every person watches, the first defeat ends the match.

One person may play alone beside observers. With no opponent, that match ends when the player is defeated.

When an observer [leaves](/systems/leaving-a-match/), the departure is announced as any other player's is. The observer's house has nothing to hand to the computer.
