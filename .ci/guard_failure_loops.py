from pathlib import Path
import re
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
ci = root / ".ci"

violations = []

# Implementation patch runners must remain inspectable. Seed parts are exempt.
for p in ci.glob("*.py"):
    if p.name in {"guard_failure_loops.py"}:
        continue
    text = p.read_text(encoding="utf-8", errors="replace")
    if "patch" in p.name.lower() or "topology" in p.name.lower() or "turbo" in p.name.lower():
        if re.search(r"base64\.(b64decode|decodebytes)|gzip\.(decompress|GzipFile)|zlib\.decompress", text):
            violations.append(f"{p}: opaque encoded/compressed implementation payload is forbidden")

# Split payload fragments are also forbidden for iterative implementation.
for p in ci.iterdir():
    n = p.name.lower()
    if ("payload" in n or "patch" in n) and re.search(r"part\d+", n):
        violations.append(f"{p}: split implementation payload is forbidden")

if violations:
    print("FAILURE-LOOP GUARD FAILED")
    for v in violations:
        print(" -", v)
    raise SystemExit(2)

print("failure-loop guard: PASS")
