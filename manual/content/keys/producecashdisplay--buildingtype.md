---
key: ProduceCashDisplay
scope: buildingtype
summary: "Shows the cash this structure pays as floating text over it."
see_also: [DisplayCreditsDelay, ProduceCashAmount, ProduceCashDelay, ProduceCashStartup]
when_omitted:
  kind: value
  value: "no, so a payment is not shown"
---

With `ProduceCashDisplay=yes`, each payment that [`ProduceCashAmount=`](/keys/producecashamount/) and [`ProduceCashDelay=`](/keys/producecashdelay/) make, and the [`ProduceCashStartup=`](/keys/producecashstartup/) bonus a captor receives, rises over the structure as text such as `+$20`. A payment that takes credits shows as `-$20`. A gain is green and a loss is red.

The text shows only when the local player owns the structure or is allied with its owner. Each structure keeps its own count: a payment made less than [`DisplayCreditsDelay=`](/keys/displaycreditsdelay/) after that structure's last text is held, and the next payment shows the sum of the held and the new amounts. Held cash is shown only by a later payment.

```ini title="rulesmd.ini"
[CAOILD] ; example BuildingType
ProduceCashDelay=900
ProduceCashAmount=50
ProduceCashDisplay=yes
```
