---
key: SecondaryFire
summary: The stage of the second weapon's upright firing animation at which an infantry type releases its round.
see_also: ["SecondaryProne", "FireUp", "Sequence", "Secondary"]
when_omitted:
  kind: value
  value: "the value of FireUp"
---

This is [`FireUp`](/keys/fireup/) for the second weapon. When a soldier fires its [`Secondary`](/keys/secondary/) weapon and its [`Sequence`](/keys/sequence/) section has a `SecondaryFire` entry, the round is released when that run reaches this stage. A type that does not set the key uses the value of `FireUp`.

A figure beyond the run's last stage is lowered to the last stage, so a one-frame run releases its round at once.
