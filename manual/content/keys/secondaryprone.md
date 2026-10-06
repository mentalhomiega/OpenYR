---
key: SecondaryProne
summary: The stage of the second weapon's prone firing animation at which an infantry type releases its round.
see_also: ["SecondaryFire", "FireProne", "Sequence", "Secondary"]
when_omitted:
  kind: value
  value: "the value of FireProne"
---

This is [`FireProne`](/keys/fireprone/) for the second weapon. When a soldier fires its [`Secondary`](/keys/secondary/) weapon and its [`Sequence`](/keys/sequence/) section has a `SecondaryProne` entry, the round is released when that run reaches this stage. A type that does not set the key uses the value of `FireProne`.

A figure beyond the run's last stage is lowered to the last stage, so a one-frame run releases its round at once.
