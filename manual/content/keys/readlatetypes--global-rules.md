---
key: ReadLateTypes
scope: global-rules
label: Read the sections of types named late
summary: Whether the section of a type that a rules key names after its type list was read is read as well.
when_omitted:
  kind: value
  value: "yes"
---

`ReadLateTypes` is an OpenTS key, read from `[General]`. With `ReadLateTypes=yes`, the default, the game reads the section of every type that a rules key names after its kind was read, in the same rules file, and then reads again any type those reads create. [Types named by other keys](/formats/rules-registries/#types-named-by-other-keys) lists which keys name a type that late.

With `no`, the game reads a type's section only when the type exists by the time its kind is read, as gamemd does. A type named later keeps the built-in defaults of its kind, and a weapon among them fires the default projectile and warhead. The setting applies to the rules file that sets it and to every file read after it.

```ini title="rulesmd.ini"
[General]
ReadLateTypes=no

[CombatDamage]
DeathWeapon=MyDeathWeapon   ; with no, [MyDeathWeapon] is not read

[MyDeathWeapon]
Damage=120
Range=5
Projectile=Invisible
Warhead=HE
```
