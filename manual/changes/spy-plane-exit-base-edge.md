---
title: Send spy planes out opposite the edge their base lies nearest, and fly planes in from that edge when Edge is omitted
category: fix
release: 0.2.0
targets:
- type: key
  id: Edge
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A spy plane now leaves over the edge opposite the one its house's construction yard lies nearest. Before, it left over the edge opposite its house's Edge. Outside a campaign, a spy plane still arrives from the house's Edge, which is north. A campaign house that omits Edge now sends its planes in from the edge nearest its construction yard, where before it used north. This also covers paradrop and airstrike planes, flying reinforcements and planes on Retreat.
