---
key: TiberiumProof
summary: Read for an infantry type, but it changes nothing.
see_also: ["system:tiberium", "Power"]
when_omitted:
  kind: value
  value: "no"
---

The game reads this flag and counts it in the rules checksum, but nothing acts on it. Infantry take no [Tiberium damage](/systems/tiberium/#damage) whether it is set or not, and the `TIBERIUM_PROOF` [veteran ability](/systems/veterancy/#abilities) has no effect either.
