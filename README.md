# Custom Retical Red Dot Project (v1)

Overview
This project contains PCB design files, manufacturing output (gerbers), and firmware for a compact red-dot aiming module intended for optics/targeting experiments. The firmware lives under .

Files & structure
-  — microcontroller firmware and test utilities.
- ,  — manufacturing outputs (may include gerber archives and board assembly notes).

Tools & build
- Firmware: typically built with PlatformIO or the Arduino toolchain; consult  for details and dependency list.
- PCB: standard gerber viewers or CAD tools used for review and fabrication.

Hardware
- Microcontroller: see  for target MCU (typically small 8/32-bit MCU entries).
- Optics: mounting and power requirements are documented near mechanical and board files.
