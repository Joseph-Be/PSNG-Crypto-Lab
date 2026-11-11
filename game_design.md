# Crypto Key Audit — game design

A linear, graded CyberRangeCZ game based on the SEED *Pseudo Random Number
Generation Lab*. The student audits an insecure, time-seeded key generator,
proves it is breakable by recovering an encrypted document, and then learns
to generate keys correctly from `/dev/urandom`.

* **Total score:** 20 points
* **Hint penalties:** between 0.5 and 2 points (per hint)
* **Unique flags per student:** yes — via APG variant sandboxes (see README)
* **Workstation:** single SEED Ubuntu 20.04 host (image `seed-ubuntu-20.04`, flavor `m1.medium`)

## Disclaimer

For educational use only. The story is fictitious.

## Attribution

Lab content adapted from the **SEED Pseudo Random Number Generation Lab**,
Copyright © 2018 Wenliang Du (CC BY-NC-SA 4.0). Scenario structure based on
the CyberRangeCZ **junior-hacker** template (Masaryk University, CC BY 4.0).
See `ATTRIBUTION.txt`.

---

## Level 1 — Task 1: The developer's shortcut (3 pts)

**Learning outcome:** The student sees that seeding `rand()` with `time(NULL)`
(or not seeding it at all) makes the key predictable and reproducible.

**Task:** Read, compile and run `genkey.c`. Observe that the key tracks the
printed timestamp; then comment out `srand(time(NULL))` and observe the key
become the fixed glibc seed-1 sequence (`67c6697351ff4aec29cdbaabf2fbe346`).
Reproduce that constant key and decrypt the planted `sample.enc` note.

**Flag source:** token `l2_flag`, planted inside `sample.enc`
(AES-128-CBC, seed-1 key, IV `000102…0f`).

---

## Level 2 — Task 2: Guessing the key (8 pts) — core level

**Learning outcome:** The student performs a known-plaintext brute-force
attack across the 2-hour seed window to recover a weak AES-128 key.

**Task:** `secret_report.enc` was encrypted with a `genkey` key seeded by
`time()` within the 2 hours before 2018-04-17 23:08:49 UTC. Given the known
16-byte PDF header, the stored IV, and the file's first ciphertext block, the
student loops over the 7200 candidate seeds, regenerates each key
(`srand(seed)` + `rand()%256`), and matches the encrypted known-plaintext
block. On a hit, they decrypt the full report.

**Flag source:** token `l3_flag`, planted inside the decrypted report. The key
used per sandbox is `genkey(1524006529 - l3_seed_offset)`, so both the key and
the flag differ for every student. Reference solver: `provisioning/roles/
workstation/files/solve.c` (instructor only — never deployed to the box).

---

## Level 3 — Task 3: How much randomness does the kernel have? (2 pts)

**Learning outcome:** The student learns where the kernel's entropy estimate
lives and which physical activity increases it.

**Task:** Read `/proc/sys/kernel/random/entropy_avail`, watch it change under
mouse/keyboard/disk activity, then run `./entropy_report.sh`.

**Flag source:** token `l4_flag`, printed by `entropy_report.sh`.

---

## Level 4 — Task 4: /dev/random, blocking, and a DoS (3 pts)

**Learning outcome:** The student contrasts the blocking `/dev/random` with
non-blocking `/dev/urandom` and reasons about the resulting DoS.

**Task:** Observe the two devices and `entropy_avail`, answer (in the report)
how draining `/dev/random` stalls a server that uses it per client, then run
`./drain_demo.sh`.

**Flag source:** token `l5_flag`, printed by `drain_demo.sh`.

---

## Level 5 — Task 5: Generating keys the right way (4 pts)

**Learning outcome:** The student fixes the flaw by reading a key directly
from `/dev/urandom`, and checks source quality with `ent`.

**Task:** Modify the provided snippet to produce a **256-bit** key (32 bytes,
64 hex chars), verify `/dev/urandom` quality with `ent` (~7.99 bits/byte), and
submit the key to the Desktop oracle `./verify_key`.

**Flag source:** token `l6_flag`, released by `verify_key` once the key is a
valid 64-hex (256-bit) value and the entropy check passes.
