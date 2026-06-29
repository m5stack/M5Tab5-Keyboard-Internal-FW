# M5Tab5-Keyboard-Internal-FW

## Overview

### SKU: A164

Internal firmware for the M5Stack Tab5 Keyboard. Tab5 Keyboard is a 70-key physical keyboard expansion module designed for Tab5. It uses an STM32F030C8T6 MCU for keyboard matrix scanning, RGB LED control, and I2C communication with the host.

The default keyboard application firmware communicates over I2C address `0x6D` and supports Normal, HID, and Character data packet modes. The project also includes an IAP firmware project and protocol/user manual documents.

## Repository Contents

- `code/Keyboard_APP`: STM32CubeIDE application firmware project for the keyboard runtime.
- `code/Keyboard_IAP`: STM32CubeIDE IAP firmware project.
- `firmware`: Prebuilt firmware image.
- `protocol`: I2C protocol documents and user manuals in English and Chinese.

## Hardware

- MCU: STM32F030C8T6
- Keys: 70-key keyboard, 14 x 5 matrix
- Interface: I2C with interrupt pin
- Default application I2C address: `0x6D`
- RGB LED: 2 x WS2812E-1313

## Related Link

- [Document & Datasheet](https://docs.m5stack.com/en/tab5/Tab5_Keyboard)
- [M5Tab5-Keyboard-UserDemo](https://github.com/m5stack/M5Tab5-Keyboard-UserDemo)

## Development

Open the firmware projects with STM32CubeIDE:

- `code/Keyboard_APP/Keyboard_APP.ioc`
- `code/Keyboard_IAP/Keyboard_IAP.ioc`

The projects target `STM32F030C8Tx` and use STM32Cube FW_F0.

## Firmware

Prebuilt firmware is available in:

- `firmware/A164-Tab5-Keyboard-STM32F030C8-FW-TAO-V1-39aabc6a-20260506_0x8000000.hex`

## Protocol

The protocol and user manual files are available in:

- `protocol/Tab5_Keyboard-I2C-Protocol-EN-V1.0.pdf`
- `protocol/Tab5_Keyboard-I2C-Protocol-CN-V1.0.pdf`
- `protocol/Tab5_Keyboard_User_Manual_EN.pdf`
- `protocol/Tab5_Keyboard_User_Manual_CN.pdf`

## License

- [M5Tab5-Keyboard-Internal-FW - MIT](LICENSE)
