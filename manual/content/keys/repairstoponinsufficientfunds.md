---
key: RepairStopOnInsufficientFunds
summary: "With no, a human player's structure that runs out of money for repair keeps repair on and carries on once the player can pay."
see_also: [RepairRate, RepairPercent, RepairStep, ClickRepairable]
when_omitted:
  kind: value
  value: "yes"
---

`RepairStopOnInsufficientFunds` is an Ares key. A repairing structure pays for each repair step as it comes. With `yes`, a step the owner cannot pay for turns repair off, and the player has to order the repair again.

With `no`, a human player's structure keeps repair on when a step cannot be paid. The repair wrench stays, the structure gains no strength while the money is short, and repair carries on at the next step the player can pay for. Computer players stop repairing as with `yes`.

```ini title="rulesmd.ini"
[General]
RepairStopOnInsufficientFunds=no
```
