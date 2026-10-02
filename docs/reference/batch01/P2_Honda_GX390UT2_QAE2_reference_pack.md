# P2 Reference Pack — Honda GX390UT2 QAE2
**Research freeze:** 2026-10-01

> **Use rule:** Documented equipment values are inputs/bounds; published performance is a validation target. No physical dimension, compression ratio, timing, or known hardware value may be altered merely to force power, torque, RPM or sound.

## Evidence grades
- **D** — documented for the locked configuration / exact spec or serial interval.
- **F** — documented family or compatible hardware; exact locked fitment/performance not proven.
- **R** — derived from documented values; formula shown.
- **C** — estimate/calibration value. **No C-grade values are introduced in this pack.**
- **U** — unresolved. Blank by design; do not guess.

## Configuration lock
**Lock status:** strong

| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Exact model/type | GX390UT2 QAE2 |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)<br>[Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) |  |
| Locked serial range | GCBCT-1000001–1114708 |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 RECOIL STARTER (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references) — Ref. 1; serial 1000001–1114708 (OEM dealer parts frontend)<br>[Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) |  |
| Calendar production years |  |  | **U** |  | Suggested source: Honda serial-number production-date record. |
| Market | U.S.A. |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 RECOIL STARTER (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references) — Ref. 1; serial 1000001–1114708 (OEM dealer parts frontend) | Recoil assembly explicitly tagged FOR U.S.A. |
| PTO family | Q type |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Rating basis | SAE J1349 net |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Fuel system | Keihin/Honda float carburetor 16100-Z5T-901, code BE88A A |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) |  |
| Starting hardware | 31210-ZE3-033 electric starter + 28400-Z5T-305ZA recoil |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 STARTER MOTOR (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E2100B/references) — Ref. 4; serial 1000001–9999999 (OEM dealer parts frontend)<br>[Honda Power Dealer — GX390UT2 QAE2 RECOIL STARTER (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references) — Ref. 1; serial 1000001–1114708 (OEM dealer parts frontend) |  |

### Configuration discipline
Only the locked variant is eligible for **D**. Family manuals/brochures are **F** unless they explicitly bind the locked type/spec/serial. Historical exact-spec documents that conflict with the present lock are preserved in **Do not confuse with**, not averaged or merged.

## 1. Performance and rating basis
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Net power | 8.7 | kW @ 3600 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)<br>[Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) | SAE J1349 production-engine net rating. |
| Continuous rated power | 7.0 | kW @ 3600 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Maximum net torque | 26.5 | N·m @ 2500 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)<br>[Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) |  |
| Rated torque derived from 8.7 kW @ 3600 | 23.077 | N·m | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `T=P/ω=8700/(2π·3600/60)`. |
| Idle speed | 1400 | rpm ±150 | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Maximum no-load governed speed | 3850 | rpm ±150 | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Service setting/limit; do not call this a redline. |
| Recommended operating speed range | 2000–3600 | rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Electronic rev limiter cut speed |  | rpm | **U** |  | Honda states the GX240–390 digital CDI includes a rev limiter, but no cut threshold is published on the cited model page. Suggested source: Honda ignition-system engineering data / CDI module specification. |
| Governor type | mechanical centrifugal |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Governor droop/regulation |  | % | **U** |  | Suggested source: Honda OEM governor performance/test specification for GX390UT2 QAE2. |
| Rating standard | SAE J1349 net |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Rating hardware basis | manufacturer production muffler and air cleaner in place |  | **F** | [Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) | Honda GX390 family/model page; not QAE2-suffix-specific. |
| Rating barometer |  | kPa | **U** |  | Suggested source: SAE J1349 test report or Honda homologation test sheet. |
| Rating inlet temperature |  | °C | **U** |  | Suggested source: SAE J1349 test report or Honda homologation test sheet. |
| Rating humidity |  | % RH / vapour pressure | **U** |  | Suggested source: SAE J1349 test report or Honda homologation test sheet. |
| Stated production tolerance |  | % | **U** |  | Honda warns mass-production engines may vary, but gives no numeric tolerance. Suggested source: Honda engine rating certification/test report. |
| Full-load fuel consumption at continuous rating | 3.5 | L/h @ 7.0 kW, 3600 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Volumetric specific fuel consumption | 0.5 | L/kWh | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `3.5 L/h ÷ 7.0 kW`. |
| BSFC at continuous rating |  | g/kWh | **U** |  | Do not convert L/h to mass without a documented test-fuel density. Suggested source: Honda dyno/fuel-consumption test sheet with fuel density/specification. |
| Part-load fuel consumption/BSFC |  |  | **U** |  | Suggested source: Honda OEM application/dyno test report. |
| Motoring/friction power or FMEP |  |  | **U** |  | Suggested source: Honda technical paper / motoring dyno test. |

## 2. Physical-bounds checks
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Cylinder compression | 0.51–0.69 | MPa @ 600 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Service compression-pressure check; not peak firing pressure. |
| Airflow at rated speed |  | kg/s or m³/s | **U** |  | Suggested source: Honda OEM dyno intake-airflow test or calibrated flow bench. |
| Exhaust gas temperature |  | °C | **U** |  | Suggested source: OEM emissions/durability test report. |
| Full-load AFR/lambda |  |  | **U** |  | Suggested source: Honda emissions calibration/dyno report. |
| Peak firing pressure |  | MPa | **U** |  | Suggested source: Honda combustion-development/SAE paper. |
| Maximum exhaust backpressure |  | kPa | **U** |  | Suggested source: Honda OEM installation/application manual. |
| Maximum intake restriction |  | kPa | **U** |  | Suggested source: Honda OEM installation/application manual. |
| Boost/manifold pressure | naturally aspirated |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |

## 3. Geometry
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Cylinder count/layout | 1, inline/single cylinder; cylinder axis inclined 25° |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Bore | 88.0 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Stroke | 64.0 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Displacement | 389 | cm³ | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Geometric displacement check | 389.256 | cm³ | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `π/4·(0.088 m)^2·0.064 m`. |
| Compression ratio | 8.2 ± 0.2:1 |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Firing order | not applicable — one cylinder |  | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `single documented cylinder ⇒ no multi-cylinder firing-order sequence`. |
| Connecting-rod centre-to-centre length |  | mm | **U** |  | Suggested source: Honda dimensional overhaul drawing / connecting-rod manufacturing drawing. |
| Compression height |  | mm | **U** |  | Suggested source: Honda piston manufacturing drawing. |
| Deck height |  | mm | **U** |  | Suggested source: Honda crankcase dimensional drawing. |
| Combustion-chamber volume/geometry |  | cm³ | **U** |  | Suggested source: Honda cylinder-head drawing or measured casting. |
| Crankpin diameter | 35.975–35.985 | mm new | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Crank main journal diameters |  | mm | **U** |  | The cited truncated shop-manual service table gives crankpin diameter but not a clean main-journal OD value. Suggested source: Full GX390UT2 overhaul manual crankshaft/bearing pages. |
| Main/rod journal widths |  | mm | **U** |  | Suggested source: Honda crankshaft manufacturing drawing. |
| Number of main bearings | 2 |  | **F** | [Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) | Honda model page states ball-bearing-supported crankshaft; exact bearing arrangement should still be confirmed from the QAE2 parts illustration. |

## 4. Valvetrain and breathing
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Valve arrangement | OHV, one intake + one exhaust valve |  | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `shop manual specifies separate IN and EX valve clearances/stems for the single cylinder`. |
| Valves per cylinder | 2 |  | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `1 intake + 1 exhaust`. |
| Intake valve head diameter |  | mm | **U** |  | Suggested source: Honda cylinder-head/valve dimensional drawing; aftermarket figures conflict. |
| Exhaust valve head diameter |  | mm | **U** |  | Suggested source: Honda cylinder-head/valve dimensional drawing; aftermarket figures conflict. |
| Valve guide ID | 6.600–6.615 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Valve stem OD intake | 6.575–6.590 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Valve stem OD exhaust | 6.535–6.550 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Valve seat width | 1.0–1.2 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Valve lash intake | 0.15 ± 0.02 | mm cold/service setting | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Valve lash exhaust | 0.20 ± 0.02 | mm cold/service setting | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Cam height intake | 32.498–32.698 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Cam height is not valve lift. |
| Cam height exhaust | 31.985–32.185 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Cam height is not valve lift. |
| Maximum valve lift |  | mm | **U** |  | Do not substitute cam height for lift. Suggested source: Honda cam profile drawing or direct measurement. |
| Cam timing IVO/IVC/EVO/EVC |  | deg crank | **U** |  | Suggested source: Honda camshaft timing diagram/cam card. |
| Duration at 1 mm / 0.050 in |  | deg | **U** |  | Suggested source: Honda cam profile drawing or measured cam card. |
| Lobe separation |  | deg | **U** |  | Suggested source: Honda cam profile drawing. |
| Follower type | slipper-type valve lifter, pushrod/rocker OHV train |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Shop-manual lubrication table names valve-lifter shaft/slipper, pushrod and rocker arm. |
| Variable timing/lift | none documented for valves |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Variable timing in this engine is ignition timing, not valve timing. |
| Port/throat diameters |  | mm | **U** |  | Suggested source: Honda head casting drawing or measured port geometry. |
| Head flow versus lift |  | cfm @ test depression | **U** |  | Suggested source: Reputable GX390 cylinder-head flow-bench report with depression stated. |

## 5. Intake and exhaust
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Carburetor architecture | horizontal butterfly/float type |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)<br>[Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) |  |
| Exact carburetor assembly | 16100-Z5T-901, BE88A A |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) | Valid for locked serial interval; later QAE2 carburetors differ. |
| Carburetor throat/venturi diameter |  | mm | **U** |  | Suggested source: Keihin/Honda BE88A dimensional drawing. |
| Carburetor flow rating |  | cfm @ test depression | **U** |  | Suggested source: Keihin/Honda carburetor flow specification. |
| Exact air-cleaner configuration |  |  | **U** |  | GX390UT2 supports several cleaner types; QAE2 exact assembly not locked by the cited general shop manual. Suggested source: QAE2 Air Cleaner parts illustration for the locked serial interval. |
| Intake runner length/area/plenum |  |  | **U** |  | Suggested source: QAE2 intake/carb insulator drawings or direct measurement. |
| Muffler/exhaust type |  |  | **U** |  | Honda J1349 model rating uses a production muffler, but exact QAE2 muffler assembly/geometry is not resolved here. Suggested source: QAE2 Muffler parts illustration + dimensional measurement. |
| Exhaust primary/pipe dimensions |  | mm | **U** |  | Suggested source: QAE2 muffler/exhaust engineering drawing. |

## 6. Fuel and ignition
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Fuel | unleaded gasoline, pump octane 86 or higher |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Carburetor assembly | 16100-Z5T-901 BE88A A |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) |  |
| Main nozzle | 16166-Z5T-901 listed among QAE2-compatible nozzle entries |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend) | The parts page lists alternative nozzles too; this does not identify the calibrated jet size. |
| Main jet calibration |  | jet number | **U** |  | Exact BE88A-A jet selection is not stated in the cited QAE2 page; shop-manual p.2-3 lists other carb codes, not BE88A. Suggested source: Honda carburetor calibration sheet for 16100-Z5T-901 / BE88A A and locked serial interval. |
| Pilot jet/screw calibration |  |  | **U** |  | Suggested source: Honda BE88A A carburetor calibration sheet. |
| Fuel pressure | gravity feed / float carburetor |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | No injection fuel rail. |
| Full-load target AFR |  |  | **U** |  | Suggested source: Honda emissions/dyno calibration report. |
| Ignition system | CDI magneto, digital/variable timing family |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Manual explicitly specifies CDI magneto; Honda model page describes variable-timing digital CDI. |
| Base ignition timing | 10 | °BTDC @ 1400 rpm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Spark advance range | 10–22 | °BTDC | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Ignition timing versus rpm/load schedule |  | °BTDC | **U** |  | Only range and one base point are published. Suggested source: Honda CDI advance map / ignition module engineering specification. |
| Spark plug | NGK BPR6ES / DENSO W20EPR-U |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Spark plug gap | 0.7–0.8 | mm | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |

## 7. Forced induction
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Forced induction | not applicable — naturally aspirated |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Turbocharger/supercharger data |  |  | **U** |  | Not applicable to locked configuration. |

## 8. Mechanical and dynamics
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Dry weight, Q PTO type | 31.7 | kg | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | QAE2 is explicitly a Q-type PTO in the type table. |
| Overall dimensions, Q PTO type | 405 × 460 × 448 | mm L×W×H | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| PTO shaft type | Q type, straight shaft |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)<br>[Honda Engines — GX340/390 model page](https://engines.honda.com/models/model-detail/x-large-gx) — GX390 Specifications; Performance Curves; SAE J1349 note (OEM primary) |  |
| Rotating inertia |  | kg·m² | **U** |  | Suggested source: Honda flywheel/crank inertial-property drawing or torsional analysis data. |
| Flywheel mass |  | kg | **U** |  | Suggested source: Honda QAE2 flywheel drawing/BOM with mass. |
| Piston mass |  | kg | **U** |  | Suggested source: Honda production piston drawing or weighing. |
| Connecting-rod mass |  | kg | **U** |  | Suggested source: Honda production rod drawing or weighing. |
| Lubrication | forced splash |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Oil capacity | 1.1 | L | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Recommended oil | SAE 10W-30, API SE or later (general use) |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Cooling | forced air |  | **D** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) |  |
| Starter motor unit | 31210-ZE3-033 |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 STARTER MOTOR (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E2100B/references) — Ref. 4; serial 1000001–9999999 (OEM dealer parts frontend) |  |
| Recoil starter assembly | 28400-Z5T-305ZA, R280 Power Red (U.S.) |  | **D** | [Honda Power Dealer — GX390UT2 QAE2 RECOIL STARTER (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references) — Ref. 1; serial 1000001–1114708 (OEM dealer parts frontend) | Reason locked serial upper bound is 1114708. |
| Cranking speed |  | rpm | **U** |  | Suggested source: Honda starter-system test spec under defined battery voltage/temperature. |
| Mean piston speed @ 3600 rpm | 7.68 | m/s | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `2·stroke·rpm/60`. |
| Peak-torque BMEP | 0.856 | MPa | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `4πT/Vd for four-stroke`. |
| Rated-point BMEP | 0.745 | MPa | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `4πT/Vd; T derived from 8.7 kW @ 3600`. |

## 9. Sound references
| Parameter | Value | Unit | Grade | Source / locator | Notes |
|---|---:|---|:---:|---|---|
| Exact locked-engine recording with known stock exhaust, rpm and load |  | URL | **U** |  | No recording met all provenance requirements in this pass. Suggested source: Bench recording of GX390UT2 QAE2 with production muffler; tachometer/logged load and microphone distance documented. |
| Combustion-event fundamental at 3600 rpm | 30.0 | Hz | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `1 cylinder × 3600 rpm ÷ 120 for a four-stroke`. |
| Combustion-event fundamental at 1400 rpm idle | 11.667 | Hz | **R** | [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary) | Formula: `1 cylinder × 1400 rpm ÷ 120`. |

## Performance curves
**Status:** usable with digitisation uncertainty.
Honda publishes the GX390UT2 curve graph but not a numeric table. Torque points below are approximate digitisation. Exact anchors are 26.5 N·m @ 2500 rpm and 8.7 kW @ 3600 rpm. Power is derived from digitised torque except the exact 3600-rpm anchor. Do not overfit simulator parameters to sub-percent differences in digitised points.

| rpm | Torque (N·m) | T grade | Power (kW) | P grade | BSFC (g/kWh) | Fuel rate | Fuel grade | Note |
|---:|---:|:---:|---:|:---:|---:|---:|:---:|---|
| 2000 | 24.70 | D | 5.17 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 2200 | 25.70 | D | 5.92 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 2400 | 26.30 | D | 6.61 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 2500 | 26.50 | D | 6.94 | R |  |  | U | 26.5 N·m @ 2500 rpm is an exact tabulated Honda anchor; surrounding points are digitised. |
| 2600 | 26.40 | D | 7.19 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 2800 | 26.30 | D | 7.71 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 3000 | 25.90 | D | 8.14 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 3200 | 25.20 | D | 8.44 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 3400 | 24.20 | D | 8.62 | R |  |  | U | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |
| 3600 | 23.08 | D | 8.70 | D |  | 3.5 L/h | D | Torque digitised approximately from Honda shop-manual p.1-6; ±~0.3 N·m / ±~50 rpm reading uncertainty. Power derived from torque except the exact 8.7 kW @ 3600 anchor. |

Curve source: [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)

## Unresolved

- **U:** Independent BSFC curve and part-load fuel-consumption map.
- **U:** Exact BE88A A main/pilot jet calibration for locked serial interval.
- **U:** Full ignition advance map (rpm and load), not just 10° base / 10–22° range.
- **U:** Cam timing events, true valve lift, duration and lobe separation.
- **U:** Valve head diameters and port/throat geometry from a primary drawing.
- **U:** Connecting-rod length, compression height, deck height, chamber geometry.
- **U:** Crank main-journal diameters/widths and bearing widths from full dimensional drawing.
- **U:** Airflow, AFR/lambda, EGT, peak firing pressure, intake restriction and exhaust backpressure limits.
- **U:** Component masses/inertias and starter cranking-speed curve.
- **U:** A stock-exhaust exact-QAE2 recording with documented rpm/load/microphone geometry.

## Do not confuse with

- **GX390RT2/T2 and other GX390UT2 type codes** — Same 389 cm³ engine family but PTO, controls, starting/charging, air cleaner, carburetor and emissions hardware can differ. Do not import suffix-specific hardware. **Grade F**. Source: [Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000](https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf) — pp. 1-2–1-6, 2-2–2-3, 3-10–3-11 (PDF pp. 6–10, 18–19, 38–39) (OEM primary)
- **QAE2 after GCBCT-1549653** — Carburetor assembly changes away from 16100-Z5T-901; therefore not in the locked interval. **Grade D**. Source: [Honda Power Dealer — GX390UT2 QAE2 CARBURETOR (1)](https://planopower.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references) — Ref. 9 and Ref. 11; serial applicability (OEM dealer parts frontend)
- **QAE2 after GCBCT-1114708** — U.S. recoil assembly changes from Power Red 28400-Z5T-305ZA to black 28400-Z5T-305ZB; excluded to keep the lock exact. **Grade D**. Source: [Honda Power Dealer — GX390UT2 QAE2 RECOIL STARTER (1)](https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references) — Ref. 1; serial 1000001–1114708 (OEM dealer parts frontend)

## Calibration implications

- The Honda pack is strong enough to validate **rated power, peak torque, idle/governed no-load speed, fuel-volume rate at continuous rating, geometry, compression and ignition anchors**.
- Do not infer BSFC from 3.5 L/h without documented fuel density. Do not tune carburetor jets or valve timing until the BE88A calibration and cam data are found.
- The digitised curve is useful for shape validation, but its reading uncertainty is much larger than the exact 2500/3600-rpm anchors.

## Source-quality note

A **D/F grade describes configuration applicability, not source prestige**. OEM primary documents are preferred. Exact-spec secondary parts catalogs are used where the OEM public site does not expose the spec-level BOM; those entries remain explicitly identified as secondary.
