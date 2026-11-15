---
key: SW.AITargeting.Preference
scope: superweapontype
label: Preferred trigger cell for superweapon aim
summary: "Makes the computer aim this superweapon at the offensive or defensive cell a trigger set, in place of its own pick."
see_also: [SW.AITargeting, SW.AITargeting.Constraints, "system:superweapons"]
when_omitted:
  kind: computed
  note: "offensive for the Nuke, MultiMissile, LightningStorm and ParaDrop modes, defensive for ForceShield, none for the others"
---

`SW.AITargeting.Preference` is an Ares key. It takes `none`, `offensive` or `defensive`. While the named cell is set, the weapon aims there in place of the cell its targeting picks:

| Value | Cell used |
|---|---|
| `offensive` | the cell the [Set preferred target cell](/mapping/actions/taction-set-preferred-target-cell/) action set, until [Clear preferred target cell](/mapping/actions/taction-clear-preferred-target-cell/) |
| `defensive` | the cell the [Set defensive target cell](/mapping/actions/taction-set-defensive-target-cell/) action set, until [Clear defensive target cell](/mapping/actions/taction-clear-defensive-target-cell/) |
| `none` | no cell; the weapon always aims by its own targeting |

For a weapon aimed by [`SW.AITargeting`](/keys/sw.aitargeting/), the preference replaces only the cell. The mode's own conditions, such as needing an enemy, and the [`SW.AITargeting.Constraints`](/keys/sw.aitargeting.constraints/) still decide whether the weapon fires, and the cell must still pass the weapon's range, designator, inhibitor and target rules. A mode that needs no cell, such as `NoTarget`, ignores the key. The same choice applies when [`SW.AutoFire`](/keys/sw.autofire/) or [`SW.UseAITargeting`](/keys/sw.useaitargeting/) aims a player's weapon.

A nuke, lightning storm, paradrop, spy plane or psychic reveal without `SW.AITargeting` follows the computer's stock rule, which aims at the offensive cell. On those weapons `none` makes the computer ignore the offensive cell, and `defensive` aims at the defensive cell instead. The force shield, psychic dominator and genetic mutator keep their stock rules whatever the key says.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AITargeting=LightningRandom
SW.AITargeting.Preference=defensive
```
