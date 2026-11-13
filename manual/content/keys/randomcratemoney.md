---
key: RandomCrateMoney
summary: "The most credits a money crate adds at random; another name for CrateMoneyBonus."
see_also: [CrateMoneyBonus, SoloCrateMoney, "system:crates"]
when_omitted:
  kind: value
  value: "900"
---

`RandomCrateMoney` is an Ares key, read from `[CrateRules]`. It is another name for the stock [`CrateMoneyBonus`](/keys/cratemoneybonus/) and sets the same number: a money crate pays its configured amount plus a random number of credits from `0` to this value. At `0` or below it pays the configured amount exactly.

If a file sets both, `RandomCrateMoney` wins.

```ini title="rulesmd.ini"
[CrateRules]
RandomCrateMoney=0
```
