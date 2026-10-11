---
key: ThirdPowerPlant
summary: Seeds the third side's RegularPowerPlant.
see_also: [RegularPowerPlant, GDIPowerPlant, "system:ai-base-building"]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[General]
ThirdPowerPlant=YAPOWR
```

The value becomes the [`RegularPowerPlant`](/keys/regularpowerplant/) of the third side in the rules' [`[Sides]`](/formats/rules-registries/) list, in each rules file that sets this key. In the stock rules that side is Yuri's. A `RegularPowerPlant=` in that side's own section of the same file overrides it, and a file that omits this key leaves the side's value alone. A computer house playing for the side inserts this plant when its [base plan runs short of power](/systems/ai-base-building/#power-and-money-interventions). [`GDIPowerPlant`](/keys/gdipowerplant/) and [`NodRegularPower`](/keys/nodregularpower/) do the same for the first two sides.
