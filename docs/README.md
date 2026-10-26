# Developer documentation

The developer guides are split by subject:

- [Building OpenTS](BUILDING.md) — supported toolchain, commands, outputs,
  build identity, and continuous integration.
- [The launcher and beta updates](LAUNCHER.md) — `OpenYR.exe`, its update
  check, the `OPENTS.INI` keys and switches it reads, and the release assets.
- [Style](STYLE.md) — source formatting, naming, C++ use, and comments.
- [History](HISTORY.md) — source lineage and reconstruction history.
- [Rationale](RATIONALE.md) — reconstruction tools, recovered structure, and
  non-obvious implementation choices.
- [Project direction](DIRECTION.md) — long-term architecture.
- [The saved game format](SAVE-FORMAT.md) — the layout of a `.SAV` file: its
  header, listing fields, compressed content, and object records.
- [OpenYR-only INI keys](OPENYR_KEYS.md) — every key we read that gamemd does
  not, grouped by file, with its default, effect and manual page.
- [INI checker](INICHECK.md) — reports rules keys the engine does not read
  and values it cannot read.
- [True-colour sprites](TRUECOLOUR.md) — PNG sheets drawn in place of SHP
  frames, with an optional house-colour mask.
- [HDR output](HDR.md) — the HDR10 back buffer, the composite pass that keeps
  the SDR colours, its settings and how it falls back to SDR.
- [Colour-blind correction](COLOURBLIND.md) — the colour-blind matrices, the Daltonize correction and its limits.
- [ts-patches in OpenTS](TS-PATCHES.md) — each CnCNet ts-patches patch and
  what OpenTS does in its place.
- [Network timing](NETCODE.md) — how a network game schedules orders, how the
  timing policy picks its values, how it compares with gamemd, and measured
  order delays.
- [Rollback netcode](ROLLBACK.md) — the rollback input link and the plan for
  porting the rest of the rollback prototype.
- [Multiplayer game modes](MULTIPLAYER_MODES.md) — the modes gamemd defines
  in `MPModesMD.ini`, the virtual slots of its mode class, and what the port
  applies from them.

See [CONTRIBUTING.md](../CONTRIBUTING.md) for contribution and review rules.
Player and modder documentation is under [manual/](../manual/README.md). When a
guide already covers a subject, link to it instead of copying the same facts.
