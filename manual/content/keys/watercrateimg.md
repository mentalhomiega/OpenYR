---
key: WaterCrateImg
summary: The OverlayType of crates that give the WaterCrate result in a campaign.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: none
---

`WaterCrateImg` names the overlay of crates that a campaign gives the [`WaterCrate`](/keys/watercrate/) result. A crate of this overlay that the map draws, or that a destroyed structure leaves as a money crate, gives that result instead of a random one. Other campaign crates are drawn at random. The OverlayType must set [`Crate=yes`](/keys/crate/) for its crates to be collected.
