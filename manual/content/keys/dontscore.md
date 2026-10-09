---
key: DontScore
summary: A victim of this type gives its killer no experience, bounty, score or kill count.
see_also: [Trainable, "system:veterancy"]
when_omitted:
  kind: value
  value: "no"
---

We skip the kill credit when a house destroys an object of a type with `DontScore=yes`. The killer earns no experience, its house receives no bounty and no score, and its kill counts do not change. The victim's house does not record the killer as the house that last hurt it. Triggers on the victim's destruction still run, and its owner still counts the loss.

The key also keeps the object out of its owner's owned count and its in-service count for its type. A structure-exists event does not see such a structure. The one exception is a vehicle, which stays in the in-service count after it leaves service.

[Earning experience](/systems/veterancy/#earning-experience) lists the other rules that decide which object receives a kill.
