---
title: Name campaigns from the string table and hide test campaigns
category: fix
release: 0.2.0
targets:
- type: key
  id: Description
  scope: campaign
  effect: changed
- type: key
  id: DebugOnly
  effect: added
credit:
- MentalHomiega
---

The mission selection list now shows each campaign's `Description=` as the text of the string table label it names, such as "Allied Campaign - Strength of Patriots" for `DESC:ALL1`, and shows a value that matches no label as written. It used to show the label itself.

A campaign whose battle file section sets `DebugOnly=yes` is now left out of the list. The shipped list used to show twelve test campaigns, each named by its first map file, after the Allied and Soviet campaigns.
