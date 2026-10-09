---
key: Crates
summary: Starting state of the crates option for a multiplayer or skirmish match.
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: "yes"
---

The value is read when the game starts and becomes the starting state of the match's crate option. The match setup, or the spawn settings for a spawned match, then sets the option for each match. Placing crates at scenario start and replacing expired crates both follow the match option alone.

:::caution[Crates=no does not stop pickup replacements]
Replacement follows the match option alone. With crates switched on for the match, a collected crate is replaced whatever this key says, and so is an expired one. With crates switched off, neither kind is replaced.

A map's own `[MultiplayerDefaults]` section can set this key for its scenario. That value does not change the match option, and it does not stop pickup replacements. [How long a crate lasts](/systems/crates/#how-long-a-crate-lasts) covers both kinds of replacement.
:::
