#import "@preview/touying:0.6.1": *
#import themes.simple: *

#show: simple-theme.with(aspect-ratio: "16-9")

= Ghidra How-To for Embedded Systems Reverse Engineering

= Introduction

== Why Reverse Engineer

- Integration: Extracting system interfaces to incorporate it into other systems
- Replication: Developing a compatible system to replace a legacy system
- Extension: Adding features to systems
- Verification: Ensure systems function as intended without "spicy extra features"
- Learning: Old systems can teach us what (not) to do

== Background on Ghidra

- Not just a decompiler - "reverse engineering framework"
- Developed by the NSA since around 2003
- Made open-source in 2019
- Extensively vetted by the security research community

== Legal Framework

- § 202c StGB: "Hackerparagraf"

  Wer eine Straftat nach § 202a oder § 202b *vorbereitet*, indem er
  1. *Passwörter oder sonstige Sicherungscodes*, ..., oder
  2. Computerprogramme, deren Zweck die Begehung einer solchen Tat ist,
  herstellt, sich oder einem anderen verschafft, ... wird mit Freiheitsstrafe bis zu zwei Jahren oder mit Geldstrafe bestraft.

== Legal Framework

- § 202a StGB

  Wer *unbefugt* sich ... Zugang zu Daten, ... die gegen unberechtigten Zugang besonders gesichert sind, ... verschafft, wird mit Freiheitsstrafe bis zu drei Jahren oder mit Geldstrafe bestraft.

- § 202b StGB

  Wer *unbefugt* sich ... nicht für ihn bestimmte Daten aus einer nichtöffentlichen Datenübermittlung ... verschafft, wird mit Freiheitsstrafe bis zu zwei Jahren oder mit Geldstrafe bestraft, ...

== Legal Framework

- § 3 GeschGehG

  Ein Geschäftsgeheimnis darf insbesondere erlangt werden durch ... ein Beobachten, Untersuchen, *Rückbauen* oder Testen eines Produkts oder Gegenstands, das oder der sich im *rechtmäßigen Besitz* des Beobachtenden, ... befindet ...

== Therefore...

- Reverse engineering is probably fine, especially if
  - It's for educational purposes
  - You have permission to do so
- Using reverse engineering to gain access to protected data (DRM, breaking communications encryption) is definitely not legal

= 01: Passcode Extraction

/*
* Learning goals:
* - Ghidra user interface overview
* - Loading files, basic analysis
* - Extracting data from decompilation
*/

== Goal

- Extract a secret (password, license key, etc.) from a Linux executable
- Given: ELF executable file (downloaded from a totally legit website)

= 02: Debug Mode

/*
* Learning goals:
* - Multiple architectures in ghidra
* - Stripped binaries
* - Adding labels
* - Setting up memory regions for peripherals
* - Cross references
*/

== Goal

- We are developing a complex control system for a revolutionary new toaster that can control the browning of the bread with millimeter precision
- One component has been outsourced to a contractor
- The controller is causing issues and we want to debug it
- Given: ELF firmware file (provided by contractor)
- They have implemented debugging features, but these are disabled in the delivered binary
- Also, the contractor has stripped all symbols from the firmware (they don't want us to know their secrets!)

== The RS2040 Microcontroller

- rv32gc (32-bit RISC-V with full base integer instruction set and MAFD extensions)
- Single hart (core)
- Running at 2 GHz
- 16KiB Flash
- 8 KiB SRAM
- 128 bidirectional GPIOs
- 4 UARTs with #strike[unlimited] 4GiB send and receive FIFO
- 4 32-bit ADCs, sampled at system clock rate

== RS2040 Memory

#table(
  columns: (1fr, 1fr, 1fr, 1fr),
  [*Region*], [*Start*], [*End*], [*Size*],
  [Flash], [0x08000000], [0x08004000], [16KiB],
  [Peripherals], [0x10000000], [0x10010000], [64KiB],
  [SRAM], [0x20000000], [0x20002000], [8KiB]
)

== RS2040 GPIO

#columns(2)[
  #table(
    columns: (1fr, 1fr),
    [*Peripheral*], [*Start*],
    [GPP0], [0x10000000],
    [GPP1], [0x10000100],
    [GPP2], [0x10000200],
    [GPP3], [0x10000300]
  )

  #v(100%)

  Registers:
  - 32bit direction (1 = out)
  - 32bit data out
  - 32bit data in
]

== RS2040 UART

#columns(2)[
  #table(
    columns: (1fr, 1fr),
    [*Peripheral*], [*Start*],
    [SRP0], [0x10001000],
    [SRP1], [0x10001100],
    [SRP2], [0x10001200],
    [SRP3], [0x10001300]
  )

  #v(100%)

  Registers:
  - 32bit baud rate (in bps)
  - 32bit bit count (5-8)
  - 32bit rx data
  - 32bit rx bytes available count
  - 32bit rx enable (0 = off)
  - 32bit tx data
  - 32bit tx ready
  - 32bit tx enable (0 = off)
]

== RS2040 ADC

#columns(2)[
  #table(
    columns: (1fr, 1fr),
    [*Peripheral*], [*Start*],
    [ADP0], [0x10002000],
    [ADP1], [0x10002100],
    [ADP2], [0x10002200],
    [ADP3], [0x10002300]
  )

  #v(100%)

  Registers:
  - 32bit enable
  - 32bit value
]

= 03: Packet Format

/*
* Learning goals:
* - Loading plain binaries
* - Memory regions in more detail
* - Data types and structures
*/

== Goal

- A competitor has released a new networked remote control product that can accelerate packets beyond light speed
- We would like to make our advanced post post post quantum encryptor compatible with their accelerator
- The specifications of the protocol aren't public, but we have access to a device that uses the protocol
- Luckily, it is also built around the RS2040 and we have access to a flash reader
- Given: BIN flash dump

= 04: State Machine

/*
* Learning goals:
* - C++ in Ghidra
* - Virtual function calls and vtables
*/

== Goal

- TODO

= Outlook

== Further Reading

- Microcorruption: https://microcorruption.com/
- Three Heads are Better Than One: https://github.com/0xAlexei/INFILTRATE2019
- GhidraClass: https://github.com/NationalSecurityAgency/ghidra/tree/master/GhidraDocs/GhidraClass
- OSDev Wiki: https://wiki.osdev.org/Expanded_Main_Page
- Reversing: Secrets of Reverse Engineering - Eldad Eilam
- Reverse Compilation Techniques - Cristina Cifuentes
- Compilers: Principles, Techniques, and Tools - Alfred V. Abo, et. al.

= Ghidra How-To for Embedded Systems Reverse Engineering
