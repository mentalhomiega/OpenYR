---
key: SW.UseAITargeting
scope: superweapontype
summary: "Fires at the cell the computer's rules pick, when a player clicks the cameo."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, clicking the charged cameo fires the weapon at once with no aiming. The shot goes to the cell that [`SW.AITargeting=`](/keys/sw.aitargeting/) picks for the house. When no cell qualifies, the weapon stays charged and the player sees [`Message.CannotFire=`](/keys/message.cannotfire/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.UseAITargeting=yes
```

See [Firing automatically](/systems/superweapons/#firing-automatically).
