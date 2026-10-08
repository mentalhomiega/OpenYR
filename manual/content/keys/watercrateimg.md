---
key: WaterCrateImg
summary: The OverlayType of crates that give the WaterCrate result in a campaign.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: none
---

`WaterCrateImg` names the overlay of crates that a campaign gives the [`WaterCrate`](/keys/watercrate/) result. A collected crate of this overlay gives that result instead of a random one. The OverlayType must set [`Crate=yes`](/keys/crate/) for its crates to be collected.
