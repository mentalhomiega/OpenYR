---
key: ReadLateTypes
scope: global-rules
label: Read the sections of types named late
summary: Whether the section of a type that a rules key names after its type list was read is read as well.
when_omitted:
  kind: value
  value: "no"
---

`ReadLateTypes` is an OpenTS key, read from `[General]`. With `ReadLateTypes=no`, the game reads a type's section only when the type exists by the time its kind is read, as gamemd does. [Types named by other keys](/formats/rules-registries/#types-named-by-other-keys) lists which keys name a type too late. Such a type keeps the built-in defaults of its kind, and a weapon among them fires the default projectile and warhead.

With `yes`, the game reads the section of every type created that late, in the same rules file, and then reads again any type those reads create. The setting applies to the rules file that sets it and to every file read after it.

```ini title="rulesmd.ini"
[General]
ReadLateTypes=yes

[CombatDamage]
DeathWeapon=MyDeathWeapon   ; with yes, [MyDeathWeapon] is read

[MyDeathWeapon]
Damage=120
Range=5
Projectile=Invisible
Warhead=HE
```
