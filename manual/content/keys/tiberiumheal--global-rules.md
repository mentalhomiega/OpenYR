---
key: TiberiumHeal
scope: global-rules
label: Healing interval
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: ".0167"
---

The game reads this value and counts it in the rules checksum, but nothing uses it. No object heals on Tiberium, so the value changes nothing, and `0` is safe.
