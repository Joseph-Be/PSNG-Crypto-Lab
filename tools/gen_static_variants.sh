#!/bin/bash
# gen_static_variants.sh  --  static fallback helper (no APG).
# Prints N ready-to-use flag value sets, one per student/group, as CSV plus
# per-variant group_vars snippets you can paste into separate builds.
#
#   ./gen_static_variants.sh [N]      (default N = 30)
set -euo pipefail
N="${1:-30}"

rand() { head -c "$1" /dev/urandom | base64 | tr -dc 'A-Za-z0-9' | head -c "$1"; }

echo "student,l2_flag,l3_seed_offset,l3_flag,l4_flag,l5_flag,l6_flag"
for i in $(seq 1 "$N"); do
    printf "%02d,%s,%s,%s,%s,%s,%s\n" \
        "$i" \
        "$(rand 12)" \
        "$(( RANDOM % 7201 ))" \
        "$(rand 16)" \
        "$(rand 12)" \
        "$(rand 12)" \
        "$(rand 16)"
done

cat <<'EOF'

# ---------------------------------------------------------------------
# For each row above, create a build whose group_vars/all.yml contains:
#   l2_flag: "<col 2>"
#   l3_seed_offset: <col 3>
#   l3_flag: "<col 4>"
#   l4_flag: "<col 5>"
#   l5_flag: "<col 6>"
#   l6_flag: "<col 7>"
# and set the training levels' answers to those same values (static mode).
# ---------------------------------------------------------------------
EOF
