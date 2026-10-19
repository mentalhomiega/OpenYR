---
key: Message.CannotFire
scope: superweapontype
summary: "Names the message shown when cameo-click targeting finds no target."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "MSG:CannotFire"
---

A string table label, printed to the player when [`SW.UseAITargeting=yes`](/keys/sw.useaitargeting/) finds no cell that qualifies.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Message.CannotFire=MSG:CannotFire
```

See [Announcements](/systems/superweapons/#announcements).
