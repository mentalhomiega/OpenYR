---
key: EMP.Sparkles
scope: warheadtype
summary: "The animation shown on an object this warhead stuns, in place of EMPulseSparkles."
see_also: ["EMP.Duration", "EMP.Cap", EMPulseSparkles]
when_omitted:
  kind: inherited
  note: The [General] EMPulseSparkles animation.
---

An object that this warhead stuns through [`EMP.Duration`](/keys/emp.duration/) shows the animation the key names for as long as the stun lasts, in place of [`EMPulseSparkles=`](/keys/empulsesparkles/). It applies only to the hit that starts the stun; an object that is already stunned keeps the animation it has. The animation ends with the stun.

```ini title="rulesmd.ini"
[MyEMPWH] ; example Warhead
EMP.Duration=150
EMP.Sparkles=EMP_FX02
```
