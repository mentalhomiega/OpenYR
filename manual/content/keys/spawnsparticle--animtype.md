---
key: SpawnsParticle
scope: animtype
label: Animation particle
see_also: ["NumParticles", "Crater", "InfantryVirus"]
when_omitted:
  kind: value
  value: none
---

The animation puts [`NumParticles`](/keys/numparticles/#scope-animtype) particles of this type at its own position each time it reaches its largest frame. The value names a particle type from the rules, such as `VirusCloud1`. A name that matches no particle type gives the same result as an omitted key: the animation spawns nothing.

An animation whose largest frame is its first spawns the particles once, when it starts. Otherwise a looping animation spawns them again each time it reaches that frame, as [`Crater`](/keys/crater/#scope-animtype) does.

The shipped [`VIRUSD`](/keys/infantryvirus/) animation, which a virus kill leaves, sets `SpawnsParticle=VirusCloud1` with `NumParticles=3`. Each `VirusCloud1` particle poisons the infantry standing in its cell.
