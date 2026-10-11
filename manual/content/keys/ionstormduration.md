---
key: IonStormDuration
summary: Parsed storm length that the engine never uses.
no_effect: true
see_also: [IonStormWarning, "system:ion-storms"]
when_omitted:
  kind: value
  value: "20"
---

The [Ion Storm start...](/mapping/actions/taction-ion-storm-start/) trigger action and the [Ion storm start in...](/mapping/missions/tmission-ion-storm-start/) team mission take the length of their storm from their own number, in game frames. They call the [lightning storm](/systems/superweapons/#lightning-storm), whose length is [`LightningStormDuration`](/keys/lightningstormduration/) when a super weapon calls it.
