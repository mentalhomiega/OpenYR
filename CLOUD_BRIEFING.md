# Briefing for cloud sessions on mentalhomiega/OpenYR

Read this before starting work. It explains what this repository is, what you can and cannot do from a cloud session, and which tasks are wanted.

## What the project is

`mentalhomiega/OpenYR` is a fork of OpenTS (https://github.com/OpenTS-Developers/OpenTS), an open rebuild of the Tiberian Sun engine. The `yr` branch extends it to run Red Alert 2: Yuri's Revenge (gamemd.exe 1.001) from its own game data. Each change ports a piece of Yuri's Revenge behaviour, such as a rules key, a weapon rule or a computer-player script action, and documents it in the manual.

Work on the `yr` branch. Run `git log upstream/main..yr` (add the upstream remote first if needed) to see what this fork adds; there are about 180 commits.

## Read first

- `AGENTS.md`, `code/AGENTS.md` and `manual/AGENTS.md`: the project's rules for code, comments, prose and the manual. Follow them.
- `CONTRIBUTING.md`, `docs/STYLE.md` and `docs/BUILDING.md`.

## What a cloud session cannot do

- **Run the game.** The game files and `gamemd.exe` are proprietary and are not in the repository. You cannot launch Yuri's Revenge, run the in-game autotests, record footage or play-test anything.
- **Read the original code.** The disassembly and decompiled output of `gamemd.exe` stay on the owner's machine. Comments citing addresses such as `(TechnoClass::GetFireError, 0x6FC339)` point at that evidence; you cannot check them.
- **Rely on a build.** The supported build is Visual Studio 2022 on Windows. Whether the code builds on Linux is untested. Try it if you like, but say clearly in your report whether a build ran and how.

Anything that changes how the game behaves has to be verified later on the owner's Windows machine with the real game. Treat your behaviour findings as proposals.

## Hard rules

- Never add game assets, original binaries, decompiled code or build output to the repository.
- Do not open pull requests, issues or comments on the upstream OpenTS project. The project forbids AI-written communication there; the owner submits anything upstream personally.
- Commit messages: an imperative subject of at most 72 characters, no body, and no `Co-authored-by` or other AI-attribution lines (see `AGENTS.md`).
- Do not rewrite history on `yr` or force-push. Push your work to a new branch, for example `cloud/review-1`, so the owner can merge it after checking.
- Every behaviour change needs its manual update: the key or system page, plus a record in `manual/changes/` (release `0.2.0`, `credit: [Lucas]`). Run `python manual/tools/manage.py update` and `python manual/tools/manage.py check` from the repository root. A "site dependencies are missing" message from `check` is expected and can be ignored. Afterwards, discard the regenerated churn with `git checkout -- manual/data/commands.yaml manual/data/scripting.yaml`.

## Wanted tasks, in order of value

1. **Code review of the `yr` commits.** Look for logic bugs, missed edge cases, null pointers, and new fields missing from `Serialize` (save games) or `crc` calls. Check that comments and manual pages still match the code. Report findings with file, line, the failing case and a suggested fix. Do not push fixes for anything you cannot test; list them instead.
2. **Manual audit.** For manual pages changed on `yr` (`manual/content/keys/`, `manual/content/systems/`, `manual/changes/`), check each claim against the source and flag or narrow any claim the code does not support. Follow `manual/AGENTS.md` and the prose rules in `AGENTS.md`.
3. **Unit tests that need no game files.** Add tests for self-contained logic, for example weapon choice (`TechnoClass::What_Weapon_Should_I_Use` and `Naval_Weapon`), mission numbering (`code/mission.hh`, `code/_mission.cpp`) and rules defaults. Use the existing test setup; no test may load game data.
4. **Mechanical cleanup.** Remove unused constants, stale comments and dead code. Keep each cleanup in its own commit, separate from behaviour changes.

## What to report back

End each session with a short report covering:

- the branch you pushed, if any, and what each commit does;
- review findings that still need a fix, each with file and line;
- what you ran (build, tests, `manage.py check`) and the results, and what you did not run.

The owner will bring the report back to the local session to fix and verify the findings against the real game.
