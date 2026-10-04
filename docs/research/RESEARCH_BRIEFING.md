# Research briefing: Ares and Phobos compatibility

This briefing drives a regular cloud research session. Its work is research documents and one tool, not engine changes. The hard rules in `CLOUD_BRIEFING.md` at the repository root apply here too; read that file first.

## Goal

The owner wants the improved build of OpenYR to run mods written for Ares and Phobos, the two Yuri's Revenge extensions most mods depend on, with Mental Omega as the long-term target. Before any of that is built, the project needs to know which tags exist, what each does, where it would hook into this engine, how hard it is, and which ones mods actually use.

Phobos comes first. The owner knows that the next major version of Mental Omega will rely heavily on Phobos, so finish the Phobos catalogue across every category, and rank Phobos systems high in `priorities.md`, alongside the Ares families the current version uses most.

How each extension may be used:

- **Ares: documentation and observed behaviour only.** Never disassemble, decompile or otherwise reverse engineer `Ares.dll`. Its behaviour is learned from its documentation, and later from comparison tests the owner runs with the real Ares on their PC.
- **Phobos: read its source freely** to understand exactly what a tag does, and describe that behaviour. Whether Phobos code may be copied into this project is still the owner's decision (Phobos is GPL-3.0 only, this project GPL-3.0 or later); until then, write your own code from the behaviour.

## Sources

- Phobos (https://github.com/Phobos-developers/Phobos) is open source under GPL-3.0, the same licence as this project. Read its documentation and source to understand each tag. Describe behaviour in your own words and link the page or file; do not copy its code into this repository.
- Ares is closed source. Use only its public documentation.
- ModEnc (https://modenc.renegadeprojects.com) documents the stock Yuri's Revenge keys.
- This repository: `manual/data/ini-keys.yaml` lists every key the engine reads now, and `code/` shows where each system lives.

Treat everything as unverified until the owner tests it with the real game. Say where a source is unclear or two sources disagree.

## Outputs

All files go under `docs/research/`.

1. `phobos-tags.md` and `ares-tags.md`: one table each. Columns: tag, the section or type it belongs to, what it does in one sentence, a source link, the files in `code/` it would touch, a size estimate (small, medium or large), and other tags it depends on. Group by system (weapons, warheads, superweapons, AI, interface and so on). Mark a tag this engine already reads.
2. `priorities.md`: a ranked plan. Put first the tags that are cheap and widely used, then the ones that unlock whole mods. Explain each ranking in a sentence.
3. `tools/mod_scan.py`: a command-line tool the owner runs on their PC against a mod folder. It reads the mod's INI files (rulesmd, artmd and the files they include), lists every section key, and counts each key by origin: read by this engine, an Ares tag, a Phobos tag, or unknown. It prints the counts and a coverage percentage. Use only the Python standard library. It must never copy the mod's files or their values anywhere; it prints key names and counts only. Add unit tests with small made-up INI files in `docs/research/tools/tests/`.

Build these up across sessions; one session will not finish the tables.

## Findings from the owner's scan (2026-10-04)

The owner ran the scanner over Mental Omega on their PC. Its rules sit inside the mod's `expandmo*.mix` archives (as `rulesmo.ini`, `artmo.ini` and `aimo.ini`), not as loose files. Of about 4,460 distinct keys, the engine reads 32%; 12% are stock Yuri's Revenge keys the engine does not read yet; 11% matched the Ares list and 1% the Phobos list; 44% were unknown.

Nearly all the unknown keys are Ares tags missing from `ares.txt`. By prefix, the most used are `Foundation.*` and `FoundationOutline.*` (custom building shapes), `SW.*` (superweapon extensions), `UC.*` (units passing through buildings), `EVA.*`, `IronCurtain.*`, `IvanBomb.*`, `Message.*`, `ForceShield.*`, `ParaDrop.*`, `SpyEffect.*` and `Money.*`. Catalogue these Ares families next, and rank them high in `priorities.md`.

Two changes to `mod_scan.py`, with tests:

1. Read `rulesmd.ini`, `artmd.ini`, `aimd.ini` and their `mo` counterparts from unencrypted MIX archives in the folder, in memory, when no loose file exists. Use the Westwood file ID (CRC-32 of the upper-case name, padded as the MIX format requires). Skip encrypted archives.
2. Add the origin "stock Yuri's Revenge, not read by this engine", for keys in the stock rules files that `engine.txt` does not list. The scanner cannot read the stock files on a cloud machine, so take the list as an optional file argument and test it with a made-up file.

## Working across sessions

- Before starting, read the logs on earlier research branches: `git branch -r --list 'origin/cloud/research-*'`, then `git show <branch>:docs/research/log/` and the files in it. Start from the newest branch's documents so work is not repeated.
- Work on a new branch `cloud/research-YYYY-MM-DD` (today's UTC date). Begin it from the newest earlier research branch, merged with `yr`, so the documents grow from session to session.
- Before finishing, add `docs/research/log/YYYY-MM-DD.md`: what you added, what is next, and questions for the owner.
- Never push to `yr` or `main`, never force-push, and never open issues, pull requests or comments anywhere.

## Report

End with a short report: the branch, what each commit adds, how far each table has got (for example "weapons and warheads done, 140 tags"), and what the next session should start with.
