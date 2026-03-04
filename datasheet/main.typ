#set page(margin: 2cm)
#set text(font: "New Computer Modern", size: 10pt)
#set heading(numbering: "1.1")
#set table(stroke: 0.5pt)

#align(center)[
  #text(size: 24pt, weight: "bold")[RS2040 Microcontroller]
  #v(0.5em)
  #text(size: 14pt)[Datasheet]
  #v(1em)
  #text(size: 10pt, style: "italic")[Preliminary --- Rev. 0.1]
]

#v(2em)

= Overview

The RS2040 is a 32-bit RISC-V microcontroller designed for embedded control applications. It features a single-hart rv32gc core running at 2 GHz, 16 KiB of Flash memory, 8 KiB of SRAM, and a rich set of peripherals including GPIO, UART, and ADC.

== Key Features

- *Core:* rv32gc (RV32I base integer ISA with M, A, F, D, C extensions)
- *Harts:* 1 (single core)
- *Clock Speed:* 2 GHz
- *Flash:* 16 KiB
- *SRAM:* 8 KiB
- *GPIO:* 128 bidirectional I/Os (4 ports × 32 pins)
- *UART:* 4 channels with 4 GiB send and receive FIFO each
- *ADC:* 4 × 32-bit ADCs, sampled at system clock rate

= Memory Map

#table(
  columns: (1fr, 1fr, 1fr, 1fr),
  align: (left, left, left, left),
  table.header[*Region*][*Start Address*][*End Address*][*Size*],
  [Flash],       [`0x0800_0000`], [`0x0800_3FFF`], [16 KiB],
  [Peripherals], [`0x1000_0000`], [`0x1000_FFFF`], [64 KiB],
  [SRAM],        [`0x2000_0000`], [`0x2000_1FFF`], [8 KiB],
)

= Peripherals

== Peripheral Address Map

#table(
  columns: (1fr, 1fr, 1fr),
  align: (left, left, left),
  table.header[*Peripheral*][*Instance*][*Base Address*],
  [GPIO Port 0], [GPP0], [`0x1000_0000`],
  [GPIO Port 1], [GPP1], [`0x1000_0100`],
  [GPIO Port 2], [GPP2], [`0x1000_0200`],
  [GPIO Port 3], [GPP3], [`0x1000_0300`],
  [UART 0],      [SRP0], [`0x1000_1000`],
  [UART 1],      [SRP1], [`0x1000_1100`],
  [UART 2],      [SRP2], [`0x1000_1200`],
  [UART 3],      [SRP3], [`0x1000_1300`],
  [ADC 0],       [ADP0], [`0x1000_2000`],
  [ADC 1],       [ADP1], [`0x1000_2100`],
  [ADC 2],       [ADP2], [`0x1000_2200`],
  [ADC 3],       [ADP3], [`0x1000_2300`],
)

== GPIO (GPPx)

Each GPIO port provides 32 bidirectional I/O pins. Pin direction and output values are controlled through memory-mapped registers. Four GPIO ports are available, providing a total of 128 I/O pins.

=== Register Map

#table(
  columns: (1fr, 1fr, 1fr, 2fr),
  align: (left, left, left, left),
  table.header[*Offset*][*Name*][*Width*][*Description*],
  [`0x00`], [`DIRECTION`], [32-bit], [Pin direction configuration. Each bit corresponds to one pin. `0` = input, `1` = output.],
  [`0x04`], [`DATA_OUT`],  [32-bit], [Output data register. Writing a bit sets the corresponding output pin level.],
  [`0x08`], [`DATA_IN`],   [32-bit], [Input data register (read-only). Reading returns the current level of each pin.],
)

== UART (SRPx)

Each UART channel provides full-duplex asynchronous serial communication with configurable baud rate and data width. Each channel has a 4 GiB transmit and receive FIFO.

=== Register Map

#table(
  columns: (1fr, 1fr, 1fr, 2fr),
  align: (left, left, left, left),
  table.header[*Offset*][*Name*][*Width*][*Description*],
  [`0x00`], [`BAUD_RATE`],  [32-bit], [Baud rate in bits per second.],
  [`0x04`], [`BIT_COUNT`],  [32-bit], [Number of data bits per frame. Valid values: 5--8.],
  [`0x08`], [`RX_DATA`],    [32-bit], [Receive data register. Reads return the next byte from the receive FIFO.],
  [`0x0C`], [`RX_AVAIL`],   [32-bit], [Number of bytes available in the receive FIFO.],
  [`0x10`], [`RX_ENABLE`],  [32-bit], [Receiver enable. `0` = disabled, non-zero = enabled.],
  [`0x14`], [`TX_DATA`],    [32-bit], [Transmit data register. Writing a byte pushes it into the transmit FIFO.],
  [`0x18`], [`TX_READY`],   [32-bit], [Transmit ready flag. Non-zero when the transmit FIFO can accept data.],
  [`0x1C`], [`TX_ENABLE`],  [32-bit], [Transmitter enable. `0` = disabled, non-zero = enabled.],
)

== ADC (ADPx)

Each ADC provides 32-bit analog-to-digital conversion, sampled continuously at the system clock rate (2 GHz).

=== Register Map

#table(
  columns: (1fr, 1fr, 1fr, 2fr),
  align: (left, left, left, left),
  table.header[*Offset*][*Name*][*Width*][*Description*],
  [`0x00`], [`ENABLE`], [32-bit], [ADC enable. `0` = disabled, non-zero = enabled.],
  [`0x04`], [`VALUE`],  [32-bit], [ADC conversion result (read-only). Contains the most recent sample.],
)
