#!/bin/bash
# make_sample.sh  (PROVISIONING ONLY)  --  Level 2 sample file.
#
# Encrypts a short note (containing the per-sandbox Level 2 flag) with the
# CONSTANT key that results when genkey.c is run WITHOUT srand(), i.e. with
# the glibc default seed of 1.  Students reproduce that key by commenting
# out the srand() line, then decrypt this note.
#
#   $1 = flag token   $2 = output path   $3 = keygen_seed binary
set -euo pipefail
FLAG="$1"; OUT="$2"; KEYGEN="$3"

IV="000102030405060708090a0b0c0d0e0f"
KEY="$("$KEYGEN" 1)"            # seed 1 == "srand() never called"

WORK="$(mktemp -d)"; PT="$WORK/note.txt"
cat > "$PT" <<EOF
Developer scratch note
-----------------------
Reminder to self: while testing I commented out the srand() line, and the
"random" key came out IDENTICAL every single run. Handy for debugging, so
I encrypted this note with that key (AES-128-CBC, IV below) to prove it.

IV = ${IV}

FLAG: ${FLAG}
EOF

openssl enc -aes-128-cbc -nosalt -K "$KEY" -iv "$IV" -in "$PT" -out "$OUT"
rm -rf "$WORK"
echo "made $OUT (seed-1 key=$KEY)"
