---
key: Message.Abort
scope: superweapontype
summary: "Replaces the refusal message of a lightning storm or psychic dominator."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The stock refusal message"
---

A string table label, printed to the player whose lightning storm or psychic dominator shot is refused because another is already active.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.Abort=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
