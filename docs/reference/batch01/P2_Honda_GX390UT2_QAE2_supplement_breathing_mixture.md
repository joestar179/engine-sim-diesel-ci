# P2 Honda GX390UT2 QAE2 — Supplement: breathing hardware and mixture

**Research freeze:** 2026-10-01

## Configuration and evidence discipline

Locked configuration: **Honda GX390UT2 QAE2, GCBCT-1000001 to 1114708, U.S. market, carburettor 16100-Z5T-901 / BE88A A, SAE J1349 net 8.7 kW @ 3600 rpm.**

- **D** — documented for this exact type/serial range.
- **F** — documented for the GX390 family or explicitly named GX390-pattern hardware; exact locked fitment/profile is unproven.
- **R** — derived from documented values; formula is shown.
- **U** — unresolved. Value intentionally blank; no guessing.
- **No C-grade/estimated values are used in this supplement.**

> **Critical Part B limitation:** no public serial-number → model-year → EPA/CARB engine-family crosswalk was found for GCBCT-1000001–1114708. The annual 3892 family names and CARB values below are therefore F-grade family-series evidence, not D-grade identification of the locked engine. A physical emissions label or Honda serial-to-certification crosswalk is needed to close this.

## Configuration lock

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Exact engine type | GX390UT2 QAE2 |  | **D** | Honda Power Products Support Publications — GX390UT1 GX390UT2 GX390UT2X General Purpose Engine Parts Catalog, 10Z5T601DP — catalog applicability: GX390UT2 GCBCT-1000001–9999999; QAE2 listed — https://publications.powerequipment.honda.com/details/10Z5T601DP | Locked supplement configuration. |
| Locked serial interval | GCBCT-1000001 to GCBCT-1114708 |  | **D** | Honda Power Products Support Publications — GX390UT1 GX390UT2 GX390UT2X General Purpose Engine Parts Catalog, 10Z5T601DP — catalog applicability: GX390UT2 GCBCT-1000001–9999999; QAE2 listed — https://publications.powerequipment.honda.com/details/10Z5T601DP | The Honda catalog covers QAE2 throughout GCBCT-1000001–9999999; this supplement intentionally narrows it to the earlier interval specified by the parent pack. |
| Market | U.S. |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 RECOIL STARTER (1) — Ref. 1: 28400-Z5T-305ZA 'FOR U.S.A.', serial 1000001–1114708 — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1100B/references | Exact serial-bounded U.S.-market recoil assembly covers 1000001–1114708, matching the locked interval. |
| Carburettor | 16100-Z5T-901, code BE88A A |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 CARBURETOR (1) — Ref. 9 carburetor and Ref. 11 main-nozzle entries; serial ranges shown — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references | Fiche applicability 1000001–1549653 includes the entire locked interval. |
| Net rating | 8.7 @ 3600 | kW @ rpm | **D** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — specifications/performance pages — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | SAE J1349 net rating; repeated here only to bind the supplement. |

## Part A1 — Valves

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Intake valve head diameter | 33.0 | mm | **F** | Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | GX390UT2 family. This conflicts with older Honda GX340/GX390 assembly information giving 35 mm; see Conflicts. |
| Exhaust valve head diameter | 31.0 | mm | **F** | Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | GX390UT2 family. |
| Intake valve stem OD | 6.575–6.590 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Standard new/service-spec dimension. |
| Exhaust valve stem OD | 6.535–6.550 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Standard new/service-spec dimension. |
| Valve-guide ID, intake/exhaust | 6.600–6.615 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Useful as a port-end geometry bound, but not the port throat diameter. |
| Valve seat contact angle | 45 | deg | **F** | Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | UT2 seat-reconditioning procedure finishes with a 45° cutter; this documents seat contact angle, not port-throat geometry. |
| Valve seat width | 1.0–1.2 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf; Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | Standard contact width; service limit 2.0 mm. |
| Intake valve overall length |  | mm | **U** |  | Not found in Honda UT2 service literature searched. Best next source: Honda production valve drawing/BOM for the exact intake-valve part number in the QAE2 serial-specific head fiche. |
| Exhaust valve overall length |  | mm | **U** |  | Not found in Honda UT2 service literature searched. Best next source: Honda production valve drawing/BOM for the exact exhaust-valve part number in the QAE2 serial-specific head fiche. |
| Intake port throat / inner-seat diameter |  | mm | **U** |  | No Honda drawing with the machined throat diameter found. Best next source: cylinder-head casting/machining drawing or a measured untouched QAE2 head. |
| Exhaust port throat / inner-seat diameter |  | mm | **U** |  | No Honda drawing with the machined throat diameter found. Best next source: cylinder-head casting/machining drawing or a measured untouched QAE2 head. |

## Part A2 — Valve lift and cam geometry

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Honda shop-manual cam height, intake | 32.498–32.698 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Cam height; NOT lobe lift. |
| Honda shop-manual cam height, exhaust | 31.985–32.185 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Cam height; NOT lobe lift. |
| Honda nominal cam base-circle diameter |  | mm | **U** |  | Not found. The 1.028 in figure in the racing rules below is a minimum tech limit, not a Honda nominal base-circle dimension. |
| Honda lobe lift corresponding to the official cam heights, intake |  | mm | **U** |  | Cannot compute cam-height minus base-circle because the matching Honda base-circle diameter was not found. |
| Honda lobe lift corresponding to the official cam heights, exhaust |  | mm | **U** |  | Cannot compute cam-height minus base-circle because the matching Honda base-circle diameter was not found. |
| Stock-Honda-pattern cam lobe lift, intake | 0.26 | in | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | VMCRA/Dillon rule explicitly requires an OEM stock Honda cam and checks profile at the lifter. Applicability to GX390UT2/QAE2 is unproven. |
| Stock-Honda-pattern cam lobe lift, exhaust | 0.241 | in | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Same racing-rule source; profile checked at lifter; not QAE2-proven. |
| Stock-Honda-pattern cam lobe lift, intake | 6.604 | mm | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.260 in × 25.4 mm/in = 6.604 mm. Derived from an F-grade source. |
| Stock-Honda-pattern cam lobe lift, exhaust | 6.1214 | mm | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.241 in × 25.4 mm/in = 6.1214 mm. Derived from an F-grade source. |
| Valve lift, intake | 0.322 | in | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Measured at retainer, rocker included, 'with valve lash as ran in race'. Not zero lash and not documented at Honda's 0.15 mm cold lash. |
| Valve lift, exhaust | 0.292 | in | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Measured at retainer, rocker included, 'with valve lash as ran in race'. Not zero lash and not documented at Honda's 0.20 mm cold lash. |
| Valve lift, intake | 8.1788 | mm | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.322 in × 25.4 = 8.1788 mm. Derived from F-grade stock-Honda-pattern data. |
| Valve lift, exhaust | 7.4168 | mm | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.292 in × 25.4 = 7.4168 mm. Derived from F-grade stock-Honda-pattern data. |
| Effective valve/lobe lift ratio, intake | 1.2385 | ratio | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.322 / 0.260 = 1.2385. This is NOT a geometric rocker-arm ratio because lash/measurement conditions differ. |
| Effective valve/lobe lift ratio, exhaust | 1.2116 | ratio | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.292 / 0.241 = 1.2116. This is NOT a geometric rocker-arm ratio. |
| Geometric rocker-arm ratio |  | ratio | **U** |  | Not found in Honda dimensional data. Do not substitute the effective lift ratios above. |
| Valve lift at Honda specified lash, intake (0.15 mm cold) |  | mm | **U** |  | No QAE2/UT2 Honda cam card or valve-lift measurement tied to the specified lash found. |
| Valve lift at Honda specified lash, exhaust (0.20 mm cold) |  | mm | **U** |  | No QAE2/UT2 Honda cam card or valve-lift measurement tied to the specified lash found. |

## Part A3 — Cam timing and automatic decompressor

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| IVO at 0.050 in lifter lift | 5 ATDC | deg crank | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Stock-Honda-pattern racing-rule profile; ±5° tech tolerance. Not proven for GX390UT2 QAE2. |
| IVC at 0.050 in lifter lift | 37 ABDC | deg crank | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Stock-Honda-pattern racing-rule profile; ±5° tech tolerance. Not proven for GX390UT2 QAE2. |
| EVO at 0.050 in lifter lift | 30 BBDC | deg crank | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Stock-Honda-pattern racing-rule profile; ±5° tech tolerance. Not proven for GX390UT2 QAE2. |
| EVC at 0.050 in lifter lift | 10 BTDC | deg crank | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Stock-Honda-pattern racing-rule profile; ±5° tech tolerance. Not proven for GX390UT2 QAE2. |
| Intake duration at 0.050 in | 212 | deg crank | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 180 − 5 + 37 = 212°. Derived from F-grade 0.050-in lifter events. |
| Exhaust duration at 0.050 in | 200 | deg crank | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 30 + 180 − 10 = 200°. Derived from F-grade 0.050-in lifter events. |
| Intake event-midpoint centreline at 0.050 in | 111 ATDC | deg crank | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: opening 5° ATDC + 212°/2 = 111° ATDC. Event midpoint; not a directly measured peak-lift centreline. |
| Exhaust event-midpoint centreline at 0.050 in | 110 BTDC | deg crank | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: EVO 30° BBDC corresponds to 150° ATDC; +200°/2 = 250° ATDC = 110° BTDC. Event midpoint. |
| Event-midpoint lobe-separation angle | 110.5 | deg cam/crank-centreline convention | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: (111 + 110) / 2 = 110.5°. Derived from F-grade 0.050-in events; do not treat as Honda QAE2 cam drawing data. |
| Cam timing at advertised/seat lift |  | deg crank | **U** |  | No Honda GX390UT2/QAE2 advertised timing card found. |
| Cam timing at 0.004 in |  | deg crank | **U** |  | No source found. |
| Cam timing at 1 mm |  | deg crank | **U** |  | No source found. |
| Automatic decompressor (EZ-Spin) event | 66 ABDC at 0.049 in | deg crank / in | **F** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Race-rule stock OEM Honda cam check; measurement location is in the cam-profile/lifter checking context. Not proven QAE2. |
| Automatic decompressor indicated lift | 1.2446 | mm | **R** | Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Formula: 0.049 in × 25.4 = 1.2446 mm. Do not reinterpret as valve lift. |
| Automatic decompressor disengagement speed |  | rpm | **U** |  | No Honda engineering value found. Best next source: cam/decompressor design specification or spin-speed bench test. |

## Part A4 — Carburettor 16100-Z5T-901 / BE88A A

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Carburettor assembly | 16100-Z5T-901 (BE88A A) |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 CARBURETOR (1) — Ref. 9 carburetor and Ref. 11 main-nozzle entries; serial ranges shown — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references | Exact QAE2, serial 1000001–1549653, fully covering the locked interval. |
| Throttle bore diameter |  | mm | **U** |  | No Honda/Keihin BE88A A dimensional drawing found. |
| Venturi/choke diameter |  | mm | **U** |  | No Honda/Keihin BE88A A dimensional drawing found. |
| Main jet size |  | jet number / mm | **U** |  | The GX390UT2 shop-manual carb table lists BE85/BE89/BE94 variants, not BE88A A. No adjacent jet size is imported. |
| Main air jet |  | jet number / mm | **U** |  | No BE88A A air-jet calibration sheet found. |
| Float height | 13.2 | mm | **F** | Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | GX390UT2 family carburettor service value. This is float height, not measured fuel-surface height in the bowl. |
| Float-bowl fuel level |  | mm from datum | **U** |  | No running/static fuel-level specification found; do not substitute float height. |
| Main nozzle entries on exact QAE2 fiche | 16166-ZF6-V00; 16166-Z5T-WC1; 16166-Z5T-901 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 CARBURETOR (1) — Ref. 9 carburetor and Ref. 11 main-nozzle entries; serial ranges shown — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references | The exact-type fiche lists multiple main-nozzle alternatives across the catalog. It does not by itself establish jet orifice diameter or which nozzle is fitted to every engine in the locked range. |
| Altitude compensation availability | Altitude Kit appears in EPA 3892AB/AC family-series records |  | **F** | EPA OTAQ Small Spark-Ignition Engines certification dataset — Honda subset (secondary mirror) — dataset last updated 2021-04-15; 3892AB/AC annual family-series rows; altitude-kit entries — https://www.scribd.com/document/512466272/Honda-EPA-Data | Family-series evidence only; no exact BE88A A altitude jet size or QAE2 serial mapping was recovered. |
| Altitude/emissions jet sizes |  | jet number / mm | **U** |  | Best next source: Honda high-altitude carburetor bulletin or BE88A A carburetor calibration sheet keyed to 16100-Z5T-901. |

## Part A5 — Intake and exhaust paths

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Exact air-cleaner inner element | 17210-ZE3-505 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 AIR CLEANER (4) — Refs. 1–3, serial 1000001–9999999 — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1503A/references | QAE2, serial 1000001–9999999. |
| Exact air-cleaner outer filter | 17218-ZE3-000 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 AIR CLEANER (4) — Refs. 1–3, serial 1000001–9999999 — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1503A/references | QAE2, serial 1000001–9999999. Exact fiche proves a two-element service assembly; material/flow curve not stated. |
| Air-cleaner housing cover | 17231-Z5T-000 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 AIR CLEANER (4) — Refs. 1–3, serial 1000001–9999999 — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1503A/references | QAE2, serial 1000001–9999999. |
| Air-cleaner restriction |  | kPa / inH2O | **U** |  | No published restriction-vs-flow curve found. Best next source: Honda application engineering or filter supplier flow test. |
| Intake port diameter |  | mm | **U** |  | No untouched-QAE2 head port drawing found. |
| Intake port length |  | mm | **U** |  | No untouched-QAE2 head port drawing found. |
| Carburettor insulator part | 16211-ZF6-000 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 CARBURETOR (1) — Ref. 9 carburetor and Ref. 11 main-nozzle entries; serial ranges shown — https://maritimesolutions.marinedealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1400H/references | Exact QAE2 fiche. Geometry is not given. |
| Carburettor insulator thickness |  | mm | **U** |  | Best next source: Honda 16211-ZF6-000 production drawing or direct measurement. |
| Carburettor insulator bore |  | mm | **U** |  | Best next source: Honda 16211-ZF6-000 production drawing or direct measurement. |
| Intake spacer part number / thickness / bore |  |  | **U** |  | A serial-specific dimensional drawing was not found; do not transfer spacer geometry from another GX390 type. |
| Production/catalog muffler | 18310-Z5T-010 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 MUFFLER (1) — Ref. 2 muffler; Ref. 5 exhaust pipe; serial ranges shown — https://geraldjoneshonda.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1600E/references; PartsWarehouse — Honda GX390UT2 QAE2 exact-type parts listing — Muffler/exhaust group — https://www.partswarehouse.com/Honda-GX390UT2-QAE2-Engine-s/513006.htm | Exact QAE2 catalog listing across 1000001–9999999. The same model's broader catalog can expose alternate muffler options; see Conflicts. |
| Exhaust pipe | 18331-Z5T-000 |  | **D** | Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 MUFFLER (1) — Ref. 2 muffler; Ref. 5 exhaust pipe; serial ranges shown — https://geraldjoneshonda.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1600E/references | Exact QAE2 catalog listing. |
| Muffler internal volume |  | L | **U** |  | No drawing or cutaway dimensions found. |
| Muffler outlet area |  | mm² | **U** |  | No production drawing found. |
| Muffler outlet diameter |  | mm | **U** |  | No production drawing found. |
| GX340/GX390 allowable/measured-system back pressure at WOT, 3000 rpm | 4.6–10.5 | kPa | **F** | Honda GX240/GX270/GX340/GX390 (UT2/RT2) Technical Manual ©2010 American Honda — p. 9, exhaust-system/back-pressure table and 30 mm measurement-location figure — https://www.trictools.com/wp-content/uploads/2015/07/HONDA-GX-390-TECH-MANUAL.pdf | Honda technical manual; measured 30 mm from exhaust-pipe mounting flange. This is the published operating range/requirement, not a measured value of muffler 18310-Z5T-010 alone. |
| GX340/GX390 allowable/measured-system back pressure at WOT, 3600 rpm | 6.0–12.5 | kPa | **F** | Honda GX240/GX270/GX340/GX390 (UT2/RT2) Technical Manual ©2010 American Honda — p. 9, exhaust-system/back-pressure table and 30 mm measurement-location figure — https://www.trictools.com/wp-content/uploads/2015/07/HONDA-GX-390-TECH-MANUAL.pdf | Honda technical manual; measured 30 mm from exhaust-pipe mounting flange. Do not convert this into muffler flow coefficient without flow data. |
| Stock QAE2 muffler back pressure at rated flow |  | kPa | **U** |  | Honda publishes the GX340/GX390 back-pressure window, not the actual pressure drop of 18310-Z5T-010 at a stated exhaust mass flow. |

## Part A6 — Connecting rod

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Stock-replacement GX390 rod centre-to-centre length | 4.41 | in | **F** | ARC Racing — Honda GX340/GX390/GX420 parts, 6272 stock-replacement billet rod — 6272 ARC Billet Rods: 4.410 in center-to-center; 1.416 in crank pin; .788 in wrist bore — https://arcracing.com/honda-gx340-gx390-gx420-parts/ | ARC 6272 is explicitly described as a stock-replacement rod for Honda GX390; not Honda primary dimensional data. |
| Stock-replacement GX390 rod centre-to-centre length | 112.014 | mm | **R** | ARC Racing — Honda GX340/GX390/GX420 parts, 6272 stock-replacement billet rod — 6272 ARC Billet Rods: 4.410 in center-to-center; 1.416 in crank pin; .788 in wrist bore — https://arcracing.com/honda-gx340-gx390-gx420-parts/ | Formula: 4.410 in × 25.4 = 112.014 mm. Applicability inherits the F-grade stock-replacement evidence. |
| Connecting-rod big-end bore / matching crank-pin basis | 1.416 | in | **F** | ARC Racing — Honda GX340/GX390/GX420 parts, 6272 stock-replacement billet rod — 6272 ARC Billet Rods: 4.410 in center-to-center; 1.416 in crank pin; .788 in wrist bore — https://arcracing.com/honda-gx340-gx390-gx420-parts/ | ARC stock-replacement design statement; corroborates Honda crankpin class but is not big-end width. |
| Connecting-rod big-end width |  | mm | **U** |  | Honda's 0.1–0.4 mm big-end side clearance is not rod width. Best next source: Honda connecting-rod drawing or direct measurement. |

## Part A7 — Published measured breathing/thermal data

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Stock GX390 intake airflow at stated speed |  | g/s or CFM | **U** |  | No Honda or technical-paper measurement found that could be tied to an unmodified stock GX390 with test conditions. |
| Stock GX390 cylinder-head flow bench versus lift |  | CFM @ stated depression | **U** |  | No reputable published stock-head flow table with lift and test depression was located. |
| Stock GX390 volumetric efficiency |  | % | **U** |  | No measured stock value located. Do not insert assumed VE. |
| Stock GX390 exhaust-gas temperature |  | °C | **U** |  | No Honda or suitably documented stock-engine technical-paper EGT data located. |
| GX340/GX390 exhaust back pressure measurement location | 30 | mm from exhaust-pipe mounting flange | **F** | Honda GX240/GX270/GX340/GX390 (UT2/RT2) Technical Manual ©2010 American Honda — p. 9, exhaust-system/back-pressure table and 30 mm measurement-location figure — https://www.trictools.com/wp-content/uploads/2015/07/HONDA-GX-390-TECH-MANUAL.pdf | Measurement geometry associated with the published back-pressure table. |

## Part B1 — U.S. EPA/CARB certification-family identification

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Exact EPA/CARB engine family covering locked GCBCT-1000001–1114708 |  | family name | **U** |  | Public Honda parts data tie QAE2 to the GCBCT range, but no public serial-to-model-year/emissions-label crosswalk was found. A physical emissions label, Honda serial-to-MY record, or exact certificate model-designation crosswalk is needed before assigning AHNXS/BHNXS/CHNXS etc. |
| Honda 389 cm³ U.S. family-name pattern | AHNXS.3892xx (MY2010); BHNXS.3892xx (MY2011); CHNXS.3892xx (MY2012); DHNXS.3892xx (MY2013); EHNXS.3892xx (MY2014) | family names | **F** | California Air Resources Board — MY2010 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0462/0463 etc.; AHNXS.3892 family-series context — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2010-small-spark-ignited-engines-exhaust; California Air Resources Board — MY2011 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0547 BHNXS.3892AB and U-U-001-0548 BHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2011-small-spark-ignited-engines-exhaust; California Air Resources Board — MY2012 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0576 CHNXS.3892AB and U-U-001-0577 CHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2012-small-spark-ignited-engines-exhaust; California Air Resources Board — MY2013 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0630 DHNXS.3892AB and U-U-001-0629 DHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2013-small-spark-ignited-engines-exhaust; California Air Resources Board — MY2014 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0657 EHNXS.3892AB and U-U-001-0658 EHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2014-small-spark-ignited-engines-exhaust | Family-series context only. The locked QAE2 serial interval was not mapped to one of these model years/families. |
| EPA equipment class for 3892AB/AC series | Small SI Nonhandheld — Class II |  | **F** | EPA OTAQ Small Spark-Ignition Engines certification dataset — Honda subset (secondary mirror) — dataset last updated 2021-04-15; 3892AB/AC annual family-series rows; altitude-kit entries — https://www.scribd.com/document/512466272/Honda-EPA-Data | EPA dataset mirror; family-series context, not locked-serial proof. |

## Part B2 — U.S. candidate family-series raw certification values

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| MY2011 BHNXS.3892AB | HC+NOx 7.1; CO 368; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2011 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0547 BHNXS.3892AB and U-U-001-0548 BHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2011-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0547. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2011 BHNXS.3892AC | HC+NOx 6.8; CO 406; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2011 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0547 BHNXS.3892AB and U-U-001-0548 BHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2011-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0548. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2012 CHNXS.3892AB | HC+NOx 6.5; CO 389; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2012 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0576 CHNXS.3892AB and U-U-001-0577 CHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2012-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0576. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2012 CHNXS.3892AC | HC+NOx 6.7; CO 408; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2012 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0576 CHNXS.3892AB and U-U-001-0577 CHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2012-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0577. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2013 DHNXS.3892AB | HC+NOx 6.5; CO 389; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2013 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0630 DHNXS.3892AB and U-U-001-0629 DHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2013-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0630. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2013 DHNXS.3892AC | HC+NOx 6.7; CO 408; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2013 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0630 DHNXS.3892AB and U-U-001-0629 DHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2013-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0629. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2014 EHNXS.3892AB | HC+NOx 6.5; CO 389; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2014 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0657 EHNXS.3892AB and U-U-001-0658 EHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2014-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0657. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2014 EHNXS.3892AC | HC+NOx 6.7; CO 408; CO2 ; HC ; NOx | g/kWh | **F** | California Air Resources Board — MY2014 Small Spark-Ignited Engines, Exhaust — Honda rows U-U-001-0657 EHNXS.3892AB and U-U-001-0658 EHNXS.3892AC — https://ww2.arb.ca.gov/new-vehicle-and-engine-certification-executive-orders-my2014-small-spark-ignited-engines-exhaust | CARB certified values; EO U-U-001-0658. Exact QAE2 serial mapping unresolved. Blank pollutant fields mean not published in the cited CARB summary; they are not zero. |
| MY2012 CHNXS.3892AC fuel | Gasoline |  | **F** | CARB Executive Order U-U-001-0577 — Honda CHNXS.3892AC, MY2012 — EO cover/certification table: 389 cc, 4-stroke >225 cc, gasoline; certification HC+NOx 6.7 g/kWh, CO 408 g/kWh — https://ww2.arb.ca.gov/sites/default/files/classic/msprog/nvepb/executive_orders/EO%20Web%20Files/SSIE-EXH/2012/0001/ssie-exh_ssie-exh_uu-1-577__sdt--20111123.pdf | Explicit in EO U-U-001-0577. Do not generalize this row into an exact-QAE2 family assignment. |
| Candidate-family 'before DF' emission results |  | g/kWh | **U** |  | CARB summary exposes certified values. Best next source: EPA Small NRSI static row export/certification test-data attachment for the exact family once the family is identified. |
| Candidate-family CO2 |  | g/kWh | **U** |  | Not recovered with trustworthy row-to-family alignment from the EPA source in this pass. |
| Candidate-family HC separately |  | g/kWh | **U** |  | Not published separately in the cited CARB summary. |
| Candidate-family NOx separately |  | g/kWh | **U** |  | Not published separately in the cited CARB summary. |
| Exact certification test cycle (A/B or equivalent) |  | cycle | **U** |  | Could not safely bind a cycle to the locked QAE2. Best next source: EPA Small NRSI row export / certificate test-data sheet after exact family identification. |
| Exact certification test speed |  | rpm | **U** |  | Do not infer 3060 or 3600 rpm from the rated-speed field. |
| Exact maximum test power |  | kW | **U** |  | EPA data mirror contains 389 cm³ power/rated-speed rows, but flattened search output did not permit trustworthy row-to-family alignment. |
| Exact EPA test fuel for locked QAE2 |  |  | **U** |  | A 2012 CARB AC EO explicitly says gasoline; the exact EPA family and EPA test-fuel specification for the locked serial remain unresolved. |

## Part B3 — EU Stage V GX390 family CO2

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Honda GX390 Stage V CO2 | 743 | g/kWh | **F** | BOMAG — CO2 Emissions (Article 43(4) disclosure table) — Honda GX390 rows (e.g. BW 65 / BPR 60/65): 743 g/kWh, NRSC; Art. 43(4) explanatory note — https://www.bomag.com/ww-de/produktuebersicht/co2-emissions/; Europower EP6500T product sheet — Honda GX390 UT2-VXB9 — engine specification block: Stage V, CO2 743 g/kWh — https://www.europowergenerators.com/Europower/Tools/pdf/productsheet.php?id=EP6500T_62d84477c7962&lang=de | Published for GX390 family/UT2-VXB9 applications; not U.S. QAE2. Raw value only; no lambda/BSFC conversion. |
| EU CO2 test cycle | NRSC |  | **F** | BOMAG — CO2 Emissions (Article 43(4) disclosure table) — Honda GX390 rows (e.g. BW 65 / BPR 60/65): 743 g/kWh, NRSC; Art. 43(4) explanatory note — https://www.bomag.com/ww-de/produktuebersicht/co2-emissions/ | BOMAG Article 43(4) disclosure. NRSC = Non-Road Steady-State Cycle. |
| EU Stage V application example | GX390 UT2-VXB9, 8.2 kW @ 3000 rpm |  | **F** | Europower EP6500T product sheet — Honda GX390 UT2-VXB9 — engine specification block: Stage V, CO2 743 g/kWh — https://www.europowergenerators.com/Europower/Tools/pdf/productsheet.php?id=EP6500T_62d84477c7962&lang=de | Useful family confirmation only; not the U.S. QAE2 lock. |
| EU type-approval number for the GX390 family behind 743 g/kWh |  |  | **U** |  | Not found in a primary Honda type-approval record. Best next source: EU type-approval certificate / information document referenced by the OEM. |
| EU NRSC subcycle (G1/G2) for the 743 g/kWh parent-engine result |  |  | **U** |  | BOMAG states NRSC but not the G1/G2 subcycle in the located disclosure. |

## Part B4 — Direct mixture / AFR / lambda measurements

| Parameter | Value | Unit | Grade | Source/locator | Notes |
|---|---|---|:---:|---|---|
| Research GX390 gasoline AFR at WOT, 1700 rpm | 14.8 | :1 air/fuel by mass | **F** | Improving the combustion process by determining the optimum percentage of LPG via RSM in an SI engine running on gasoline-LPG blends — Test procedure — https://www.sciencedirect.com/science/article/pii/S0378382021002253 | Technical paper reports a 'Honda GX390' at 1700 rpm, fully open throttle. However it also describes OHC and 9.12:1 compression, unlike the locked Honda GX390UT2 OHV/8.2:1 engine. Treat only as research-engine family/pattern evidence, not stock-QAE2 mixture. |
| Directly measured stock-QAE2 AFR |  | air/fuel ratio | **U** |  | No Honda or technical-paper result tied to 16100-Z5T-901 / BE88A A and the locked serial interval was found. |
| Directly measured stock-QAE2 lambda |  | lambda | **U** |  | No qualifying direct measurement found. Per task instruction, lambda is not computed from emissions. |
| Directly measured stock-QAE2 exhaust CO |  | % vol | **U** |  | No qualifying Honda service/dyno value found at a stated speed/load. |

## Conflicts

| Parameter | Value/source A | Value/source B | Disposition |
|---|---|---|---|
| Intake valve head diameter | 33 mm — Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | 35 mm — Honda GX340/GX390 Series Engine Assembly Information — Cylinder Head/Valves assembly page — https://manualzz.com/doc/23214038/honda-gx340--gx390-series-engine-assembly-information | Do not average. 33 mm is later UT2-family documentation; 35 mm is older GX340/GX390 assembly information. Neither is promoted to D for QAE2. |
| Exhaust valve head diameter | 31 mm — Honda GX240/GX270/GX340/GX390 UT2 Overhaul and Service Manual — §12 Cylinder Head/Valves, pp. 12-4–12-10 — https://www.scribd.com/document/751592782/Honda-GX240-GX270-GX340-GX390-UT2-Overhaul-and-Service-Manual-With-Bookmarks-2 | 31 mm — Honda GX340/GX390 Series Engine Assembly Information — Cylinder Head/Valves assembly page — https://manualzz.com/doc/23214038/honda-gx340--gx390-series-engine-assembly-information | No conflict; included as a cross-check accompanying the intake-valve conflict. |
| QAE2 muffler fitment | 18310-Z5T-010 listed as MUFFLER — Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 MUFFLER (1) — Ref. 2 muffler; Ref. 5 exhaust pipe; serial ranges shown — https://geraldjoneshonda.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1600E/references; PartsWarehouse — Honda GX390UT2 QAE2 exact-type parts listing — Muffler/exhaust group — https://www.partswarehouse.com/Honda-GX390UT2-QAE2-Engine-s/513006.htm | Honda catalogs can expose alternate muffler options for the same broader QAE2 type — Honda dealer fiche — GX390UT2 QAE2 GCBCT-1000001–9999999 MUFFLER (1) — Ref. 2 muffler; Ref. 5 exhaust pipe; serial ranges shown — https://geraldjoneshonda.powerdealer.honda.com/parts/engines/engines/gx/GX390/GX390UT2-QAE2/Z5T0E1600E/references | Treat 18310-Z5T-010 as an exact cataloged part, but do not infer that every QAE2 application used one unique acoustic/exhaust option without the original equipment build record. |
| Cam geometry source | Honda UT2 manual: cam height IN 32.498–32.698 mm; EX 31.985–32.185 mm — Honda GX390RT2/GX390T2/GX390UT2 Shop Manual 62Z5F000 — service-specification table, manual pp. 2-2–2-3 (PDF pp. 18–19) — https://www.honda-engines-eu.com/files/files/shop-manual-gx390-ut2-en.pdf | Racing rule: stock-OEM-pattern maximum lifter lift IN .260 in; EX .241 in; base circle minimum 1.028 in — Dillon Motor Speedway Mini Cup Rules 2025 — VMCRA Stock OEM Honda GX390 cam profile — Engine/camshaft rule: OEM stock Honda cam; lifter profile, valve lift and EZ-Spin data — https://dillonspeedway.com/mini-cup-rules/ | Not directly reconcilable. Racing-rule base circle is a tech minimum, not Honda nominal. Do not subtract it from Honda cam height. |

## Unresolved (U)

1. **Intake valve overall length** — Not found in Honda UT2 service literature searched. Best next source: Honda production valve drawing/BOM for the exact intake-valve part number in the QAE2 serial-specific head fiche.
2. **Exhaust valve overall length** — Not found in Honda UT2 service literature searched. Best next source: Honda production valve drawing/BOM for the exact exhaust-valve part number in the QAE2 serial-specific head fiche.
3. **Intake port throat / inner-seat diameter** — No Honda drawing with the machined throat diameter found. Best next source: cylinder-head casting/machining drawing or a measured untouched QAE2 head.
4. **Exhaust port throat / inner-seat diameter** — No Honda drawing with the machined throat diameter found. Best next source: cylinder-head casting/machining drawing or a measured untouched QAE2 head.
5. **Honda nominal cam base-circle diameter** — Not found. The 1.028 in figure in the racing rules below is a minimum tech limit, not a Honda nominal base-circle dimension.
6. **Honda lobe lift corresponding to the official cam heights, intake** — Cannot compute cam-height minus base-circle because the matching Honda base-circle diameter was not found.
7. **Honda lobe lift corresponding to the official cam heights, exhaust** — Cannot compute cam-height minus base-circle because the matching Honda base-circle diameter was not found.
8. **Geometric rocker-arm ratio** — Not found in Honda dimensional data. Do not substitute the effective lift ratios above.
9. **Valve lift at Honda specified lash, intake (0.15 mm cold)** — No QAE2/UT2 Honda cam card or valve-lift measurement tied to the specified lash found.
10. **Valve lift at Honda specified lash, exhaust (0.20 mm cold)** — No QAE2/UT2 Honda cam card or valve-lift measurement tied to the specified lash found.
11. **Cam timing at advertised/seat lift** — No Honda GX390UT2/QAE2 advertised timing card found.
12. **Cam timing at 0.004 in** — No source found.
13. **Cam timing at 1 mm** — No source found.
14. **Automatic decompressor disengagement speed** — No Honda engineering value found. Best next source: cam/decompressor design specification or spin-speed bench test.
15. **Throttle bore diameter** — No Honda/Keihin BE88A A dimensional drawing found.
16. **Venturi/choke diameter** — No Honda/Keihin BE88A A dimensional drawing found.
17. **Main jet size** — The GX390UT2 shop-manual carb table lists BE85/BE89/BE94 variants, not BE88A A. No adjacent jet size is imported.
18. **Main air jet** — No BE88A A air-jet calibration sheet found.
19. **Float-bowl fuel level** — No running/static fuel-level specification found; do not substitute float height.
20. **Altitude/emissions jet sizes** — Best next source: Honda high-altitude carburetor bulletin or BE88A A carburetor calibration sheet keyed to 16100-Z5T-901.
21. **Air-cleaner restriction** — No published restriction-vs-flow curve found. Best next source: Honda application engineering or filter supplier flow test.
22. **Intake port diameter** — No untouched-QAE2 head port drawing found.
23. **Intake port length** — No untouched-QAE2 head port drawing found.
24. **Carburettor insulator thickness** — Best next source: Honda 16211-ZF6-000 production drawing or direct measurement.
25. **Carburettor insulator bore** — Best next source: Honda 16211-ZF6-000 production drawing or direct measurement.
26. **Intake spacer part number / thickness / bore** — A serial-specific dimensional drawing was not found; do not transfer spacer geometry from another GX390 type.
27. **Muffler internal volume** — No drawing or cutaway dimensions found.
28. **Muffler outlet area** — No production drawing found.
29. **Muffler outlet diameter** — No production drawing found.
30. **Stock QAE2 muffler back pressure at rated flow** — Honda publishes the GX340/GX390 back-pressure window, not the actual pressure drop of 18310-Z5T-010 at a stated exhaust mass flow.
31. **Connecting-rod big-end width** — Honda's 0.1–0.4 mm big-end side clearance is not rod width. Best next source: Honda connecting-rod drawing or direct measurement.
32. **Stock GX390 intake airflow at stated speed** — No Honda or technical-paper measurement found that could be tied to an unmodified stock GX390 with test conditions.
33. **Stock GX390 cylinder-head flow bench versus lift** — No reputable published stock-head flow table with lift and test depression was located.
34. **Stock GX390 volumetric efficiency** — No measured stock value located. Do not insert assumed VE.
35. **Stock GX390 exhaust-gas temperature** — No Honda or suitably documented stock-engine technical-paper EGT data located.
36. **Exact EPA/CARB engine family covering locked GCBCT-1000001–1114708** — Public Honda parts data tie QAE2 to the GCBCT range, but no public serial-to-model-year/emissions-label crosswalk was found. A physical emissions label, Honda serial-to-MY record, or exact certificate model-designation crosswalk is needed before assigning AHNXS/BHNXS/CHNXS etc.
37. **Candidate-family 'before DF' emission results** — CARB summary exposes certified values. Best next source: EPA Small NRSI static row export/certification test-data attachment for the exact family once the family is identified.
38. **Candidate-family CO2** — Not recovered with trustworthy row-to-family alignment from the EPA source in this pass.
39. **Candidate-family HC separately** — Not published separately in the cited CARB summary.
40. **Candidate-family NOx separately** — Not published separately in the cited CARB summary.
41. **Exact certification test cycle (A/B or equivalent)** — Could not safely bind a cycle to the locked QAE2. Best next source: EPA Small NRSI row export / certificate test-data sheet after exact family identification.
42. **Exact certification test speed** — Do not infer 3060 or 3600 rpm from the rated-speed field.
43. **Exact maximum test power** — EPA data mirror contains 389 cm³ power/rated-speed rows, but flattened search output did not permit trustworthy row-to-family alignment.
44. **Exact EPA test fuel for locked QAE2** — A 2012 CARB AC EO explicitly says gasoline; the exact EPA family and EPA test-fuel specification for the locked serial remain unresolved.
45. **EU type-approval number for the GX390 family behind 743 g/kWh** — Not found in a primary Honda type-approval record. Best next source: EU type-approval certificate / information document referenced by the OEM.
46. **EU NRSC subcycle (G1/G2) for the 743 g/kWh parent-engine result** — BOMAG states NRSC but not the G1/G2 subcycle in the located disclosure.
47. **Directly measured stock-QAE2 AFR** — No Honda or technical-paper result tied to 16100-Z5T-901 / BE88A A and the locked serial interval was found.
48. **Directly measured stock-QAE2 lambda** — No qualifying direct measurement found. Per task instruction, lambda is not computed from emissions.
49. **Directly measured stock-QAE2 exhaust CO** — No qualifying Honda service/dyno value found at a stated speed/load.

## Calibration-use notes

- The **33 mm UT2 intake-valve** result is the closest Honda family document found, but remains **F**, not D. Do not mix it with the older 35 mm GX340/GX390 value.
- The Dillon/VMCRA cam profile is useful as a **stock-Honda-pattern shape reference only**. It is not evidence that the locked QAE2 cam has those exact .050-in events or lifts.
- The Honda technical-manual back-pressure figures are **system back-pressure bounds/requirements at WOT** measured 30 mm from the exhaust flange. They are not the pressure drop of muffler 18310-Z5T-010 by itself.
- The **EPA/CARB certification values are not mixture measurements**. No lambda or BSFC is calculated from them.
- The ScienceDirect AFR result is retained only because the task asked for technical-paper measurements; its reported OHC/9.12:1 test-engine description conflicts with the locked Honda GX390UT2 architecture, so it must not calibrate QAE2 mixture.
