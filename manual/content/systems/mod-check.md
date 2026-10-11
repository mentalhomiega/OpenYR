---
title: Mod check
summary: A host refuses a player who joins a LAN game with different mods from its own, and names the mods that differ.
category: multiplayer-networking
keys: []
related:
  - type: using
    id: mods
  - type: system
    id: network-packet-validation
---

A player who asks to join a LAN game sends the mods in force on his machine, in the order the game reads them. The host compares them with its own and refuses a player whose mods differ. The refused player sees a message naming the host's mods and which of them differ, and the host sees the same comparison in the lobby's message list, headed by the player's name.

## What is compared

Each mod is sent as its folder's name with a hash of its [`mod.ini`](/formats/mod-ini/) and of the rules, art and AI overlay files that file names. The two machines must agree on all of these:

- the mods, matched by folder name without regard to case;
- the order of the mods;
- the hash of each mod.

Carriage returns are left out of the hash, so a file with Windows or Unix line endings is the same mod. A mod's other files, such as sprites, sounds and maps, are not hashed. Two players with a mod of the same name but different sprites are not told apart, and neither are two copies of one mod whose overlays match.

The message tells apart the mods a player lacks, the mods he has that the host lacks, the mods whose files differ, and mods that are the same but in a different order.

## Players with no mods

A player with no mods sends an empty list, which is also what a build without this check sends. Such a player joins a host that has no mods, and is refused by a host that has some. A host from a build without the check does not compare anything, so a player with mods can still join it.

## Long lists

The list shares a packet field of 384 characters with its terminator. A list whose text does not fit is sent as the number of mods and one hash of the whole list. Two such lists are still refused when they differ, but the message gives only the number of mods and cannot name the ones that differ.

## Games the check does not cover

A game started from a launch file, such as one a [`SPAWN.INI`](/formats/spawn-ini/) describes, has no join step, and nothing compares the players' mods.

The rules, art and AI files of the players are also checked on their own when a player joins, and a difference there is refused with a general message that does not name a mod.
