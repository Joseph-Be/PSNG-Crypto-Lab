# Crypto Key Audit — Pseudo Random Number Generation (CyberRangeCZ)

A linear, **graded (20-point)** CyberRangeCZ scenario based on the SEED
*Pseudo Random Number Generation Lab*. Students audit an insecure
time-seeded AES key generator, **recover an encrypted document** by
brute-forcing the weak key, measure kernel entropy, and learn to generate
keys correctly from `/dev/urandom`.

Built for ~30 students, with **unique flags per student** so answers cannot
be shared.

## Levels and scoring

| # | Level (SEED task) | Points | Hints (penalty) |
|---|---|---|---|
| Info | Introduction | — | — |
| Access | Get access (passkey `start`) | — | — |
| 1 | Task 1 — The developer's shortcut | 3 | 0.5, 1 |
| 2 | Task 2 — Guessing the key *(core)* | 8 | 1, 1.5, 2 |
| 3 | Task 3 — How much randomness does the kernel have? | 2 | 0.5 |
| 4 | Task 4 — /dev/random, blocking, and a DoS | 3 | 0.5, 1 |
| 5 | Task 5 — Generating keys the right way | 4 | 1, 1.5 |
| | **Total** | **20** | |

Every scored level's flag is a **per-sandbox token**, so two students never
submit the same answer.

## Topology

| Host | Image | Flavor | Role |
|---|---|---|---|
| workstation | `seed-ubuntu-20.04` | `m1.medium` | the SEED box; students do everything here |
| workstation-router | `debian-12-x86_64` | `m1.small` | connectivity to WAN (adjust/remove to taste) |

One host per sandbox keeps the pool light for 30 concurrent students.

## How the unique flags work (APG / variant sandboxes)

`variables.yml` (at the repo root) declares the per-sandbox secrets. On each
sandbox build the platform generates unique values and injects them into
Ansible provisioning, which plants them into the workstation:

| Variable | Planted as | Level answer |
|---|---|---|
| `l2_flag` | token inside `sample.enc` | Level 1 |
| `l3_seed_offset` | picks the weak key (`seed = 1524006529 − offset`) | — (provisioning only) |
| `l3_flag` | token inside `secret_report.enc` | Level 2 |
| `l4_flag` | printed by `entropy_report.sh` | Level 3 |
| `l5_flag` | printed by `drain_demo.sh` | Level 4 |
| `l6_flag` | released by `verify_key` | Level 5 |

In the training definition each scored level has **Variant Answers** enabled
with **Correct Answer – Variable Name** set to the variable above
(`answer_variable_name` in `training.json`, and `variant_sandboxes: true`).
Create the training with **Generate variant sandboxes** selected.

The method is the same for everyone (that's the teaching); only the recovered
token differs — so students still learn the attack but can't copy answers.

## ⚠️ Verify before you deploy

1. **Image name** — `topology.yml` uses `seed-ubuntu-20.04`. It must match an
   image in your OpenStack/Glance catalog exactly.
2. **Management user** — `topology.yml` sets `mgmt_user: ubuntu`. Set it to the
   cloud-init user of *your* SEED image (often `ubuntu` or `seed`), or
   provisioning SSH will fail.
3. **Student login** — the GUI/SSH login is `seed` / `dees` (provisioning sets
   this). Change in `roles/workstation/tasks/main.yml` and the Access level if
   your image differs.
4. **Tools on the image** — the box needs `gcc`, `openssl`, `ent`, `xxd`,
   `hexdump`. The standard SEED 20.04 VM ships all of these. `ent` is **not**
   in Ubuntu's default apt repo, so if you use a plain Ubuntu image install it
   yourself (it's needed by Level 5's `verify_key`).
5. **APG availability** — if your instance does **not** support variant
   sandboxes, use the static fallback below.
6. **Fractional penalties** — hint penalties are 0.5–2 as requested. If your
   platform version only accepts integer scores, multiply every `max_score`
   and `hint_penalty` by 10 (totals 200; penalties 5–20) — the ratios are
   unchanged.

## Deploy (APG path)

1. Put this directory in a Git repo your platform can reach.
2. **Sandbox Definition** → add from the repo; build the images/pool.
3. **Training Definition** → import `training.json` (or recreate the levels);
   it already has variant answers + `variant_sandboxes: true`.
4. Create a **Training Instance** with *Generate variant sandboxes*, pool size
   ≥ 30, and share the access token with students.

## Static fallback (no APG)

If variant sandboxes aren't available:

1. Run `tools/gen_static_variants.sh 30` to print 30 unique value sets.
2. For each student (or small group) create a build whose
   `provisioning/group_vars/all.yml` (copy from `all.yml.example`) holds one
   value set, and set that build's training-level answers to the same values
   (static answers, Variant Answers **off**).

This reproduces per-student uniqueness manually; APG just automates it.

## Instructor reference solution

* Weak-key brute forcer: `provisioning/roles/workstation/files/solve.c`
  (`gcc -o solve solve.c -lcrypto`; run with the file's first ciphertext block).
  **Never deployed to the student box.**
* Full per-level walkthroughs: `game_design.md` and each level's *Solution*.

## Security note

Provisioning stages the seed/key tooling under `/opt/prng-audit`, uses it to
build the encrypted files, then **deletes the whole directory** — so the seed,
the key, and the generator scripts are not readable on the student box. Those
files live only in this (instructor) repo.

## Credits & licence

Adapted from the SEED *Pseudo Random Number Generation Lab* (© 2018 Wenliang
Du, CC BY-NC-SA 4.0); structure from the CyberRangeCZ junior-hacker scenario
(Masaryk University, CC BY 4.0). Custom code under MIT. See `ATTRIBUTION.txt`
and `LICENSE`.
