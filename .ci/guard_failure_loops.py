from pathlib import Path
import re
import sys

root = Path(sys.argv[1] if len(sys.argv) > 1 else ".")
ci = root / ".ci"

violations = []

# These two predate this policy and are temporarily grandfathered until they are
# converted to readable patches. No new opaque patch runner is permitted.
legacy_opaque_exempt = {
    "turbo_arch_patch.py",
    "full_turbo_topology_patch.py",
}

# Retired mechanism from the generic-forced-induction incident. Reintroduction
# is an immediate policy failure.
retired_names = {
    "generic_forced_induction_v1_patch.py",
    "generic_v1.part00.b64",
    "generic_v1.part01.b64",
    "generic_v1.part02.b64",
    "generic_v1.part03.b64",
    "consolidated.part01.b64",
    "consolidated.part02.b64",
    "consolidated.part03.b64",
}

for name in retired_names:
    if (ci / name).exists():
        violations.append(f"{ci / name}: retired failed transport mechanism must not be present")

# Implementation patch runners must remain inspectable. Seed parts are exempt,
# as are the two named legacy runners above.
for p in ci.glob("*.py"):
    if p.name == "guard_failure_loops.py" or p.name in legacy_opaque_exempt:
        continue
    text = p.read_text(encoding="utf-8", errors="replace")
    if "patch" in p.name.lower() or "topology" in p.name.lower() or "turbo" in p.name.lower():
        if re.search(r"base64\.(b64decode|decodebytes)|gzip\.(decompress|GzipFile)|zlib\.decompress", text):
            violations.append(f"{p}: opaque encoded/compressed implementation payload is forbidden")

# Split payload fragments are forbidden for iterative implementation. Seed
# bootstrap parts are explicitly exempt.
for p in ci.iterdir():
    n = p.name.lower()
    if n.startswith("seed.part"):
        continue
    if ("payload" in n or "patch" in n or "generic_v1" in n or "consolidated" in n) and re.search(r"part\d+", n):
        violations.append(f"{p}: split implementation payload is forbidden")

if violations:
    print("FAILURE-LOOP GUARD FAILED")
    for v in violations:
        print(" -", v)
    raise SystemExit(2)

print("failure-loop guard: PASS")
