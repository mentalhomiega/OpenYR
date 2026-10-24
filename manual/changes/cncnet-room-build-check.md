---
title: Hold back a CnCNet match while a player is on another OpenYR build
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

A room in the built-in CnCNet lobby compares each player's OpenYR build with the host's. The build id is the commit the debug log prints on its second line, with "-modified" added when tracked files differ from it, followed by the architecture and the packed version that the save and network stamp uses. Before this change the room compared the version text. An official build's version text has no commit, so two official builds of one version matched and could play one match together.

A player on another build stays in the room, with a note under its name such as "different OpenYR build (abc1234)". A stock client that never says hello is listed as "not OpenYR". The host cannot start while either is in the room, even when it is ready. The player on another build sees that its build differs from the host's and that it should update OpenYR. A stock client is no longer removed 20 seconds after it joins; the host removes it with Kick Player.

Two local builds of the same commit that have different uncommitted changes get the same id, so the room cannot tell them apart. A build from before this change announces its display text, which the room shows as a different build with no commit.
