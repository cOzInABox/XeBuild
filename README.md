# xeBuild

Welcome to the official source code for the core engine and resources of **xeBuild**, a NAND re/construction tool for the Xbox 360 platform.

This codebase has been built, reverse-engineered, iterated upon, and maintained since 2010. It is being released to the public domain for the purposes of digital preservation, historical documentation, and to allow the homebrew community to address any long-standing bugs.

---

## Project Status & Intent

* **Platform:** Xbox 360 (JTAG/RGH/Devkit image construction)
* **Languages:** C, PPC asm
* **Toolchain:** Historically compiled via GCC (MSYS)

---

## License & Public Domain Dedication

This project is a composite work. The custom logic, reverse-engineered structures, and integration routines are dedicated entirely to the public domain. However, this codebase includes essential third-party cryptographic components which retain their respective terms.

### 1. Main Project Code (MIT)

To the extent possible under law, the author has dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

For full legal text, see [LICENSE](LICENSE) or [Open Source Initiative](https://opensource.org/license/mit).

### 2. Third-Party Cryptographic Libraries

Please respect the original licensing and attribution requirements embedded within the source files:

* **Helper Cryptographic Routines (libmcrypto, [BigDigits](https://di-mgt.com.au/bigdigits.html), DES, MD5, AES, SHA, RC4, CRC32):**
  * *Utilizes the libmcrypto library, which in turn uses other code under their own licenses.*
  * *Utilizes Gary S. Brown's implementation of CRC32.*

* **Thanks to the following authors for being generous with their time and work:**
  * *David Ireland, Brian Gladman, Eric Young, Dang Nguyen Duc, Gary S. Brown*
  * *Tom St Denis, and Alexander Peslyak*

See [LICENSE.txt](LICENSE.txt) for more complete license attributions from the source code.

---

## Technical Notes for Contributors

* **Clean-Room Implementation:** This codebase contains zero proprietary Microsoft SDK headers, binaries, or copyrighted system assets.
* **Data Structures:** Structure definitions and bootloader headers were meticulously reverse-engineered, manually verified against live execution environments, and implemented entirely from scratch in standard C.
* **Building:** This project was designed to be portable and lightweight, requiring minimal external dependencies to compile.
* **Build Tooling:** Makefiles are included for GCC, a VS2010 solution file is included and should import into even the latest VS

---

## Contributing

If you are a developer still active in the Xbox 360 homebrew scene, your contributions are welcome!

---

## Building

This release was originally built with makefiles but has been reworked to use CMake and Ninja.

See [cmakeit.bat](./cmakeit.bat) for convenience script and various toolchain examples capable of building it on windows.

---

*Thank you to the entire Xbox 360 homebrew and scene community for keeping the platform alive for over a decade!*

June 2026. Welcome to my nostalgia corner! Signing off...

_[cOz]_
