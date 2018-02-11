# Game Boy Emulator
A cross-platform Game Boy (DMG) emulator written in C.

#### Features
- Full emulation of Game Boy CPU, video and sound hardware
- Passes all blargg cpu/timing test roms
- Dot based renderer can handle games like Prehistorikman

#### Outstanding
- Implement serial port (required for Alleyway)
- Pinball dreams crashes (unknown cause).
- Improve the slightly dodgy sound emulation.
- Move to a threaded model.

#### Screenshots

<img src="https://mattbraz.github.io/image/oh-win7-336x326.png">&nbsp;
<img src="https://mattbraz.github.io/image/megaman-linux-320x313.png" width="336" height="326">

#### Keys

Up:     Up
Down:   Down
Left:   Left
Right:  Right
A:      Z
B:      X
Start:  S
Select: A

Build Instructions
---
A Makefile is provided that can build the software for Linux, Mac and Windows.

#### Dependencies
- SDL2
- MinGW (Windows)

Useful Links
---

