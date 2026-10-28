---
title: Treat a CnCNet room's host and listings as untrusted
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

A guest in the built-in CnCNet lobby takes the host's map only when its file name is in the guest's own map list. The name is matched without regard to case, and a name with a folder, a drive, a network path or `..` never matches. When the map is not found, the guest sees "(you do not have this map)", cannot get ready, and OpenYR opens no file the host named. Before this change the host's text was opened as a file path.

A guest starts a match only when the host's start message comes from the current host, the guest is ready, its build matches the host's, and it has the host's map with the same checksum. Any other start message is ignored. Before this change a modified host could start the match on every guest at once.

The host's numeric options are held to the ranges of the room screen's sliders: game speed 0 to 6, computer players 0 to 6, computer difficulty 0 to 2, unit count 1 to 10, tech level 1 to 10, and credits from 2500 to the rules' maximum. A guest's own game setup returns when it leaves the room, so values copied from a host no longer carry into the next skirmish or LAN game.

Names and options from other players can no longer add a line, a key or a section to `SPAWN.INI`. Control characters in them are dropped when they are read.

A game listing whose room channel does not start with the room prefix, is the broadcast channel, or holds a comma, a space or a control character is not listed. A room password cannot contain a space, a comma or a control character, or start with a colon; the lobby's Host and Join buttons stay off and the tip says why.

When the host's tunnel is named by a host name instead of an address, the lookup runs on its own thread, so the room window keeps answering. If the name cannot be found, the second game window is closed and the room shows "The tunnel ... could not be found." An address given as an IPv4 number works as before.

A guest plays a match only when the tunnel the host names is on the CnCNet tunnel list the guest has, freshly downloaded or kept from the last good download. The address and port must match a listed tunnel; a host name is looked up once and its address must match. Otherwise the guest stays in the room and the room says the host's relay is not on the CnCNet tunnel list. A guest with no tunnel list at all also stays in the room, and the room says the list could not be loaded. A host that sets `Tunnel=` to a relay that is not on this machine is held to the list too. Before this change a modified host could send every guest's match traffic to any address. A relay on this machine that the guest's own `Tunnel=` setting names, as the local test setups use, needs no list.

The tunnel list cache is written under a temporary name and moved into place, so two Host clicks close together no longer leave a half-written cache.

The host's player list and start message are kept within 430 bytes, the most a chat server relays whole. Eight players with 15-character nicks fit. When a list or start would be longer, the host is told in the room chat to use shorter names or fewer players, and the start does not go out; before this change the server cut the message and guests got a wrong relay port or an unreadable start. A guest also refuses a start message that lacks the end marker or the right seat count.

Pressing Back while the lobby is still connecting returns within about a tenth of a second. Before this change the lobby window stopped answering for 8 seconds or more.
