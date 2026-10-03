# Research briefing: Ares and Phobos compatibility

This briefing drives a twice-weekly cloud session. Its work is research documents and one tool, not engine changes. The hard rules in `CLOUD_BRIEFING.md` at the repository root apply here too; read that file first.

## Goal

The owner wants the improved build of OpenYR to run mods written for Ares and Phobos, the two Yuri's Revenge extensions most mods depend on, with Mental Omega as the long-term target. Before any of that is built, the project needs to know which tags exist, what each does, where it would hook into this engine, how hard it is, and which ones mods actually use.

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
3. `tools/mod_scan.py`: a command-line tool the owner runs on his PC against a mod folder. It reads the mod's INI files (rulesmd, artmd and the files they include), lists every section key, and counts each key by origin: read by this engine, an Ares tag, a Phobos tag, or unknown. It prints the counts and a coverage percentage. Use only the Python standard library. It must never copy the mod's files or their values anywhere; it prints key names and counts only. Add unit tests with small made-up INI files in `docs/research/tools/tests/`.

Build these up across sessions; one session will not finish the tables.

## Working across sessions

- Before starting, read the logs on earlier research branches: `git branch -r --list 'origin/cloud/research-*'`, then `git show <branch>:docs/research/log/` and the files in it. Start from the newest branch's documents so work is not repeated.
- Work on a new branch `cloud/research-YYYY-MM-DD` (today's UTC date). Begin it from the newest earlier research branch, merged with `yr`, so the documents grow from session to session.
- Before finishing, add `docs/research/log/YYYY-MM-DD.md`: what you added, what is next, and questions for the owner.
- Never push to `yr` or `main`, never force-push, and never open issues, pull requests or comments anywhere.

## Report

End with a short report: the branch, what each commit adds, how far each table has got (for example "weapons and warheads done, 140 tags"), and what the next session should start with.
