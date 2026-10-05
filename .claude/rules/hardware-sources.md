---
description: Treat the schematic files as the wiring source of truth, and read the relevant peripheral datasheet before any driver-level change.
applyTo: "**"
---

# Rule: Schematic is truth; read the datasheet before touching a driver

This BSP is wiring-driven. Two classes of reference documents govern any
hardware-facing change. Consult them **before** writing or modifying code, not
after.

## 1. Wiring / implementation source of truth

When deciding *how something is connected* — pin assignments, which bus a device
is on, shared lines, power rails, M5Bus routing, connector pinouts — use this
evidence order:

1. The checked-in official schematic assets
  [CORE2_V1.0_SCH_page_01.png](../../datasheet/CORE2_V1.0_SCH_page_01.png)
  and [core2_for_aws_sch_01.webp](../../datasheet/core2_for_aws_sch_01.webp).
  They cover the Core2 main board and the AWS-specific M5GO Bottom,
  respectively. The Core2 for AWS v1.3 replaces the add-on with the one in
  [M5GO_Bottom2_AWS_V1.3_SCH.pdf](../../datasheet/M5GO_Bottom2_AWS_V1.3_SCH.pdf);
  [CORE2_V1.0_SCH.pdf](../../datasheet/CORE2_V1.0_SCH.pdf) is the vector form
  of the main-board page. Their official URLs and verified hashes are recorded in
  [datasheet/schema.yml](../../datasheet/schema.yml).
2. [datasheet/schema.yml](../../datasheet/schema.yml) — the machine-readable,
   net-level wiring schema (core board + add-on board joined by M5Bus). This is
  the **primary implementation reference** after it has been checked against
  both source images.
3. [datasheet/schematic.md](../../datasheet/schematic.md) — the human-readable
   hardware reference derived from the same schematics.
4. [.claude/rules/pinmap.md](pinmap.md) and [.claude/rules/board.md](board.md) —
   the distilled, steering-oriented summaries.

The official product references are
<https://docs.m5stack.com/en/core/core2_for_aws> and, for v1.3,
<https://docs.m5stack.com/en/products/sku/K010-AWS-V13>. They corroborate the combined
kit and current published specifications; they are not a substitute for the
schematic assets when deciding physical wiring. This source set was last
verified on 2026-10-05.

If any of these disagree with code, **the schematic files win**. If the steering
rules (`pinmap.md` / `board.md`) disagree with [schema.yml](../../datasheet/schema.yml)
or [schematic.md](../../datasheet/schematic.md), treat that as a defect: stop,
flag it to the user, and reconcile rather than guessing.

Do not invent pin assignments, bus membership, or reset/power ownership from
generic ESP32 devkit assumptions. Derive them from the schematic sources.

## 2. Read the peripheral datasheet before driver-level changes

Before modifying or adding code that programs a specific chip (register writes,
init sequences, timing, mode configuration, addresses), read both the local
Markdown reference and its checked-in PDF/XLSX source. Markdown is the searchable
implementation aid; the original PDF/XLSX controls if they disagree. Driver-level
means anything that touches chip registers, init/wake/reset timing, or the
device's electrical behavior — not application glue.

Peripheral → datasheet map:

| Peripheral | Module | Markdown | Original source |
| --- | --- | --- | --- |
| AXP192 PMU | `lib/power` | [AXP192.md](../../lib/power/datasheet/AXP192.md) | [AXP192.pdf](../../lib/power/datasheet/AXP192.pdf), complete V1.2 register source |
| SY7088 boost | `lib/power` | [SY7088.md](../../lib/power/datasheet/SY7088.md) | [SY7088.pdf](../../lib/power/datasheet/SY7088.pdf) |
| MPU6886 IMU | `lib/motion` | [MPU-6886.md](../../lib/motion/datasheet/MPU-6886.md) | [MPU-6886.pdf](../../lib/motion/datasheet/MPU-6886.pdf) |
| BMI270 IMU (v1.3) | `lib/motion` | `BMI270.md`* | [BMI270.pdf](../../lib/motion/datasheet/BMI270.pdf), BST-BMI270-DS000-05 rev 1.3 |
| BM8563 RTC | `lib/rtc` | [BM8563.md](../../lib/rtc/datasheet/BM8563.md) | [BM8563_cn.pdf](../../lib/rtc/datasheet/BM8563_cn.pdf) |
| FT6336 touch | `lib/button`, `lib/display` | [FT6336.md](../../lib/button/datasheet/FT6336.md) | [Ft6336_cn.xlsx](../../lib/button/datasheet/Ft6336_cn.xlsx) |
| ILI9342 LCD | `lib/display` | [ILI9342.md](../../lib/display/datasheet/ILI9342.md) | [ILI9342C.pdf](../../lib/display/datasheet/ILI9342C.pdf) |
| ILI9342E LCD | `lib/display` | `ILI9342E.md`* | [ILI9342E.pdf](../../lib/display/datasheet/ILI9342E.pdf) |
| NS4168 speaker amp | `lib/audio` | [NS4168.md](../../lib/audio/datasheet/NS4168.md) | [NS4168.pdf](../../lib/audio/datasheet/NS4168.pdf) |
| SPM1423 microphone | `lib/audio` | [SPM1423.md](../../lib/audio/datasheet/SPM1423.md) | [SPM1423.pdf](../../lib/audio/datasheet/SPM1423.pdf) |
| LMD4737T261 microphone (v1.3) | `lib/audio` | `LMD4737T261.md`* | [LMD4737T261.pdf](../../lib/audio/datasheet/LMD4737T261.pdf), LinkMems M374 v1.1 (-OAC05) |
| ATECC608 secure element | `lib/crypto` | `ATECC608A-TNGTLS.md`* | [ATECC608A-TNGTLS.pdf](../../lib/crypto/datasheet/ATECC608A-TNGTLS.pdf), DS40002112B |
| 1027 vibration motor | `lib/power` | `1027RFN01.md`* | [1027RFN01.pdf](../../lib/power/datasheet/1027RFN01.pdf) |
| ESP32 SoC | board | `datasheet/ESP32.md`* | [ESP32.pdf](../../datasheet/ESP32.pdf) |

\* Machine conversions made with pymupdf4llm (PyMuPDF) on 2026-10-05, stored
next to their PDFs and excluded from the generated Doxygen site. Each
header records the source URL and SHA-256. Tables and figures can be lossy, so
check register bitfields against the PDF. The other Markdown files are curated
implementation specifications.

The LMD4737T261 PDF is the distributor copy of the -OAC05 variant; the v1.3
schematic gives no suffix and M5Stack links no microphone datasheet for v1.3.

Seven checked-in sources are byte-for-byte identical to the files currently
linked by M5Stack. `AXP192.pdf` intentionally differs: the checked-in V1.2 file
is the complete 53-page register datasheet, while the current M5Stack link is a
shorter English Revision 1.0 document from 2016. Keep that distinction explicit.

Notes:

- If a peripheral has **no** datasheet markdown in its module (e.g. the
  SK6812 LEDs, the CP2104/CH9102F USB-UART), there is no local
  datasheet to read; rely on the schematic sources above and the existing
  third-party driver/library, and say so rather than fabricating register
  details.
- When a change spans more than one chip (for example the shared-GPIO0 audio
  path), read **all** the involved datasheets first (NS4168, SPM1423 *and*
  LMD4737T261).

## 3. Workflow for any hardware-facing change

1. Identify the affected peripheral(s) and bus/pins.
2. Read [schema.yml](../../datasheet/schema.yml) /
   [schematic.md](../../datasheet/schematic.md) to confirm the wiring.
3. Read the relevant peripheral Markdown and original source (table above).
4. Make the code change consistent with both.
5. Per [.claude/rules/design-doc.md](design-doc.md), update
   [docs/design.md](../../docs/design.md) if the change affects anything the design
   doc describes.

## What not to do

- Do not change pin numbers, bus membership, or power/reset ownership without a
  citation from the schematic sources.
- Do not write or edit register-level driver code without first reading that
  chip's Markdown and original source when they exist.
- Do not resolve a source conflict by silently choosing the newer-looking file;
  record both revisions and use the source that actually defines the fitted
  hardware/register behavior.
- Do not let [pinmap.md](pinmap.md) / [board.md](board.md) silently diverge from
  [schema.yml](../../datasheet/schema.yml); reconcile and flag instead.
