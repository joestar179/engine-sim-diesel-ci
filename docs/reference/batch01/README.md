# Engine reference-data batch 01
Research freeze: 2026-10-01

Included:
- P1 Kohler/Rehlko Command PRO CH750-3005
- P2 Honda GX390UT2 QAE2, locked to GCBCT-1000001–1114708

Configuration-lock assessment:
- P2 Honda: **strong** exact type + serial interval. Carburetor and starter/recoil hardware are serial-bounded.
- P1 Kohler: **exact spec-code lock, but serial/date interval unresolved**. Current PA-CH750-3005 identity is clear; serial-dependent as-built carburetor/starter details are not. This is deliberately not hidden.

Critical data-quality finding:
- Current OEM CH750 model data are 20.1 kW (27 hp) @ 3600 rpm and 55.9 N·m @ 3200 rpm, SAE J1940 gross.
- A legacy OEM family chart is marked SAE J1490 gross and reaches 22.7 kW @ 3600 rpm. It is preserved as historical family data only and must not be used as a locked-current validation curve.
- A 2021 distributor catalog for exact CH750-3005 also says 55.9 N·m @ 2800 rpm and mis-converts 27 hp as 19.4 kW. No averaging or reconciliation was performed.

No C-grade calibration guesses were introduced.
