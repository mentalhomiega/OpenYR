---
title: Bounty
summary: Pays the owner of a destroying object an amount set by the destroyed object's type and rank, or takes it when the amount is negative.
category: buildings-economy
keys:
  - Bounty
  - Bounty.Value
  - Bounty.RookieValue
  - Bounty.VeteranValue
  - Bounty.EliteValue
  - Bounty.Display
  - BountyDisplay
  - BountyEnablers
  - GivesBounty
related:
  - type: system
    id: veterancy
  - type: system
    id: temporal-weapons
  - type: system
    id: transports
---

When an enemy object whose type sets [`Bounty=yes`](/keys/bounty/) destroys an object, the destroyer's owner receives the destroyed object's bounty. The amount is the destroyed object's [`Bounty.Value`](/keys/bounty.value/), or the key for the rank it held when it died: [`Bounty.RookieValue`](/keys/bounty.rookievalue/), [`Bounty.VeteranValue`](/keys/bounty.veteranvalue/) or [`Bounty.EliteValue`](/keys/bounty.elitevalue/). With none of them set, the amount is 0.

```ini title="rulesmd.ini"
[General]
BountyEnablers=GACNST ; example: collecting needs a construction yard

[HTNK] ; example VehicleType: the destroyer
Bounty=yes
Bounty.Display=yes

[MTNK] ; example VehicleType: the destroyed object
Bounty.Value=300
Bounty.EliteValue=500
```

Here the owner of a Rhino tank that destroys an enemy Grizzly tank receives 300 credits, or 500 for an elite Grizzly, provided the owner has a construction yard.

## Who pays

No bounty is paid when any of these holds:

- the destroyer's owner is the destroyed object's owner or one of its allies;
- the destroyed object's country sets [`GivesBounty=no`](/keys/givesbounty/);
- the destroyer's type does not set `Bounty=yes`.

## Who collects

When [`BountyEnablers`](/keys/bountyenablers/) lists structures, nothing is paid unless the destroyer's owner owns one of them. The key page says which structures count.

## Amounts

A positive amount is added to the owner's credits. A negative amount is taken from the owner's credits first. If they run out, stored Tiberium is sold at its credit value to cover the rest. The owner never goes into debt, so a negative bounty takes at most what the house has, and a house with no credits and no Tiberium loses nothing.

An amount of 0 pays and takes nothing.

## Which kills count

A bounty follows any kill that names a destroyer. That covers damage from a weapon, a vehicle crushing infantry or another crushable object, erasure by a [temporal weapon](/systems/temporal-weapons/), and passengers killed when a [transport](/systems/transports/) is destroyed, each of which pays its own bounty. A structure its owner sells pays nothing, and neither does an object that is destroyed with no destroyer.

## Display

`Bounty.Display=yes` on the destroyer's type shows the amount as `+$300` or `-$300` over the spot where the destroyed object stood. A destroyer's type that does not set `Bounty.Display` follows [`BountyDisplay`](/keys/bountydisplay/) in `[AudioVisual]`. The text rises and disappears after about 75 game frames, in the colour of the destroyer's owner.

Every player sees it, whichever house the destroyer belongs to, provided that spot is on their screen while the text lasts. Nothing is shown for an amount of 0. A negative amount is shown in full, even when the house had less to lose.
