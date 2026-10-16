---
key: WaterCrate
summary: The crate result delivered by a WaterCrateImg crate in a campaign.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: Money
---

`WaterCrate` is the result of a campaign crate whose overlay is the one [`WaterCrateImg`](/keys/watercrateimg/) names, when the map draws the crate or a destroyed structure leaves it as a money crate. [Choosing the result](/systems/crates/#in-a-campaign) lists the campaign crates that are drawn at random instead. Skirmish and multiplayer games do not use this setting.

The value is one of the [crate result](/reference/enums/crate/) tokens. An unrecognized token gives `Money`, so a misspelled result behaves exactly like `Money`.
