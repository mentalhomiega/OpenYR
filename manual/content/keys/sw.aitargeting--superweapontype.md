---
key: SW.AITargeting
scope: superweapontype
summary: "Chooses how the computer, automatic fire and cameo-click targeting pick a target."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "Each behavior has its own rule."
---

One of `None`, `Nuke`, `MultiMissile`, `LightningStorm`, `PsychicDominator`, `GeneticMutator`, `IronCurtain`, `ForceShield`, `ParaDrop`, `DropPod`, `Offensive`, `HunterSeeker`, `NoTarget`, `Stealth`, `Self`, `Base`, `EnemyBase`, `Attack`, `LowPower`, `LowPowerAttack` and `LightningRandom`. [The modes](/systems/superweapons/#ai-targeting-modes) table lists what each one picks.

`IronCurtain` and `ForceShield` leave the behavior's own computer rule in charge. A weapon that sets no mode uses the rule for its behavior, and the new behaviors default as the table shows.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AITargeting=Offensive
```

See [Ai targeting modes](/systems/superweapons/#ai-targeting-modes).
