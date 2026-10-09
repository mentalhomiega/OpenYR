# Briefing for cloud sessions on mentalhomiega/OpenYR

Read this before starting work. It explains what this repository is, what you can and cannot do from a cloud session, and how your work reaches the game.

## What the project is

`mentalhomiega/OpenYR` is a fork of OpenTS (https://github.com/OpenTS-Developers/OpenTS), an open rebuild of the Tiberian Sun engine. It extends OpenTS to run Red Alert 2: Yuri's Revenge (gamemd.exe 1.001) from its own game data. Each change ports a piece of Yuri's Revenge behaviour, such as a rules key, a weapon rule or a computer-player script action, and documents it in the manual.

Two branches matter:

- **`modern`** is the build our play-testers get, with gameplay fixes plus modern extras (menus, movies, resolution options). **Work on `modern`** and begin every branch from `origin/modern`.
- **`yr`** holds the same gameplay fixes without the modern extras and is used for strict comparison with gamemd. We carry fixes from `modern` to `yr` ourselves.

Many recent `modern` commits port gamemd behaviour that differs from what OpenTS or Vinifera chose. That is deliberate: where they disagree, gamemd wins. Do not "fix" code back towards OpenTS or Vinifera behaviour.

## Read first

- `AGENTS.md`, `code/AGENTS.md` and `manual/AGENTS.md`: the project's rules for code, comments, prose and the manual. Follow them.
- `CONTRIBUTING.md`, `docs/STYLE.md` and `docs/BUILDING.md`.
- `CLOUD_JOBS.md`: the job list. Each session does the one job it was given.

## What a cloud session cannot do

- **Run the game.** The game files and `gamemd.exe` are proprietary and are not in the repository. You cannot launch Yuri's Revenge, run the in-game autotests, record footage or play-test anything.
- **Read the original code.** The disassembly and decompiled output of `gamemd.exe` stay on our machine. Comments citing addresses such as `(TechnoClass::GetFireError, 0x6FC339)` point at that evidence; you cannot check them, so review the logic, not the citation.
- **Rely on a build.** The supported build is Visual Studio on Windows. Whether the code builds on Linux is untested. Try it if you like, but say clearly in your report whether a build ran and how.

Anything that changes how the game behaves is verified later on our Windows machine with the real game. Treat your behaviour findings as proposals.

## Hard rules

- Never add game assets, original binaries, decompiled code or build output to the repository.
- Never disassemble, decompile or otherwise reverse engineer `Ares.dll`; reimplement Ares behaviour from its documentation. Phobos source may be read; do not copy Phobos code until MentalHomiega decides on it (see `docs/research/RESEARCH_BRIEFING.md`).
- Do not open pull requests, issues or comments on the upstream OpenTS project, or anywhere else. The project forbids AI-written communication upstream; MentalHomiega submits anything upstream personally.
- Commit messages: an imperative subject of at most 72 characters, no body, and no `Co-authored-by` or other AI-attribution lines (see `AGENTS.md`).
- Never write the owner's real name anywhere: not in files, credits, commit authors or messages. Use `MentalHomiega`, and commit as `MentalHomiega <182634060+mentalhomiega@users.noreply.github.com>`.
- Never push to `modern`, `yr` or `main`, never force-push and never delete branches. Push your work to the new `cloud/*` branch your job names.
- A change that adds a saved field must add it to the class's save and load code and bump `REVISION` in `code/savever.h` by one.
- Every behaviour change needs its manual update: the key or system page, plus a record in `manual/changes/` (release `0.2.0`, `credit: [MentalHomiega]`). Run `python manual/tools/manage.py update` and `python manual/tools/manage.py check` from the repository root. A "site dependencies are missing" message from `check` is expected and can be ignored. Afterwards, discard the regenerated churn with `git checkout -- manual/data/commands.yaml manual/data/scripting.yaml`.

## Answers from the owner

MentalHomiega knows Yuri's Revenge well. These answers settle questions from earlier logs:

- **Mind-controlled units cannot enter transports or structures, except Bio Reactors.** A controlled unit entering a Bio Reactor is used up, so releasing its control node there is correct.
- **A `UnitReload=yes` pad both rearms and repairs docked aircraft**, as the code does now.

## How your branch reaches the game

Our local session fetches every new `cloud/*` branch, builds it on Windows, runs the unit tests, the 14 campaign mission wins and a replay comparison in the real game, and merges what passes into `modern` and `yr`. So:

- keep one job per branch, in small commits, each fix in its own commit with its manual update;
- say in your log which changes you could only read and not run;
- do not merge other `cloud/*` branches into yours unless your job says to.

## Session log

- Before starting, read the logs of earlier sessions on the same job: `git branch -r --list 'origin/cloud/*'`, then `git show <branch>:cloud-log/` to list a branch's logs and `git show <branch>:cloud-log/<file>` to read one. Carry forward anything they left open for your job.
- Before finishing, add `cloud-log/YYYY-MM-DD-<job>.md` (today's UTC date) to your branch in its own commit: the report below, what you started and did not finish, ideas for the next session, and questions for MentalHomiega.

## What to report back

End each session with a short report covering:

- the branch you pushed and what each commit does;
- findings that still need a fix, each with file, line, the failing case and a suggested fix;
- what you ran (build, tests, `manage.py check`) and the results, and what you did not run.
