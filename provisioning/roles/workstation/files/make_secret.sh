#!/bin/bash
# make_secret.sh  --  (PROVISIONING ONLY)  build the per-sandbox encrypted
# "confidential report" for Level 3.
#
#   $1 = seed_offset  (0..7200, the per-sandbox APG value)
#   $2 = flag token   (the per-sandbox APG value planted inside the file)
#   $3 = output path  (e.g. /home/seed/Desktop/secret_report.enc)
#   $4 = path to the compiled keygen_seed binary
#
# The file timestamp advertised to students is 2018-04-17 23:08:49 UTC
# (epoch 1524006529). The real seed is END - seed_offset, i.e. somewhere in
# the 2-hour window the students are told to search.
set -euo pipefail

OFFSET="$1"
FLAG="$2"
OUT="$3"
KEYGEN="$4"

END=1524006529
SEED=$(( END - OFFSET ))

# Known, predictable IV (stored with the file; students are given it).
IV="09080706050403020100a2b2c2d2e2f2"

# Derive the weak key for this sandbox's seed (same libc as the student).
KEY="$("$KEYGEN" "$SEED")"

# Build the plaintext. First 16 bytes are the predictable PDF-1.5 header
# (this is the known-plaintext the students exploit), followed by filler
# that looks like a tax return, with the unique flag planted inside.
WORK="$(mktemp -d)"
PT="$WORK/report.pdf"

# exactly the 16 known bytes: %PDF-1.5\n% + d0 d4 c5 d8 0a 34
printf '%%PDF-1.5\n%%\xd0\xd4\xc5\xd8\x0a\x34' > "$PT"
cat >> "$PT" <<EOF

CONFIDENTIAL -- 2018 TAX RETURN (internal copy)
Taxpayer: A. Hodler, CEO
Prepared: 2018-04-17

This document was encrypted with a key produced by the in-house
"genkey" utility. If you are reading this, that key was not as
secret as its author believed.

FLAG: ${FLAG}

-- end of document --
EOF

# Encrypt with AES-128-CBC, PKCS7 padding, no salt, explicit key+IV.
openssl enc -aes-128-cbc -nosalt -K "$KEY" -iv "$IV" -in "$PT" -out "$OUT"

# Set the advertised modification time so the story checks out.
touch -d "2018-04-17 23:08:49 UTC" "$OUT"

rm -rf "$WORK"
echo "made $OUT (seed=$SEED key=$KEY)"
