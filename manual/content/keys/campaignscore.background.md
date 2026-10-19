---
key: CampaignScore.Background
summary: The shape drawn as the background of the campaign score screen for a side.
see_also: ["CampaignScore.Animation", "CampaignScore.Palette"]
when_omitted:
  kind: computed
  note: '`ASCRBKMD.SHP` for the first side in `[Sides]`, `SSCRBKMD.SHP` for the second and `SYCRBKMD.SHP` for any other; the 640x480 pictures end in `SM`.'
---

Write the file name with its `.SHP` extension. The shape is drawn over the whole 632 by 568 screen for the side of the player who finished the mission. Pair it with [`CampaignScore.Palette`](/keys/campaignscore.palette/).
