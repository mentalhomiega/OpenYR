---
key: WaterCrate
summary: The crate result delivered by a WaterCrateImg crate in a campaign.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: Money
---

`WaterCrate` is the result of every campaign crate whose overlay is the one [`WaterCrateImg`](/keys/watercrateimg/) names. Campaign crates are not drawn at random; the overlay alone decides the result. Skirmish and multiplayer games do not use this setting.

The value is one of the [crate result](/reference/enums/crate/) tokens. An unrecognized token gives `Money`, so a misspelled result behaves exactly like `Money`.
