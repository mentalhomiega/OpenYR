---
key: AirstrikeRechargeTime
summary: "The frames between airstrike calls."
see_also: [EliteAirstrikeRechargeTime, Airstrike]
when_omitted:
  kind: value
  value: "0"
---

After the last plane of an airstrike leaves the map or is destroyed, the object cannot call another airstrike for this many frames. The count does not start when the airstrike is called. An elite object waits [`EliteAirstrikeRechargeTime`](/keys/eliteairstrikerechargetime/) instead.
