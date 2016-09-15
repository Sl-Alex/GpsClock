#LED matrix GPS wall clock

This project has the following features:
- 80x24 big LED matrix display with automatic brightness control (based on HT16K33 drivers).
- Temperature/humidity measurement.
- Air pressure monitoring/logging. Last 24 hours are always on the display in a form of a diagram.
- Non-volatile FRAM memory for the air pressure log.
- GPS module for automatic time synchronization.
- No buttons, no settings. User manual is not required.
- Automatic DST time correction. Time zone can be easily changed in the source code, no further modifications are required.

Following 3rd party HW was used:
- STM32F103C8T6 minimum system development board ([photo](./modules/stm32f103c8t6_board.jpg))
- BMP180 temperature/pressure sensor module ([photo](./modules/bmp180_module.jpg))
- BH1750 light sensor  module ([photo](./modules/bh1750_module.jpg))
- Si7021 temperature/humidity sensor  module ([photo](./modules/si7021_module.jpg))
- FM24C04 FRAM chip.
- EB-800 GPS module with 5V->3V converter.

Module interconnections are available on the ([following diagram](./schematic.png)). The only PCB you have to make is a HT16K33 module (15 pcs). KiCAD project is in the ([PCB](./PCB)) folder.

P.S. If 32kHz generation on the processor board is not stable, then it is highly recommended to make a proper crystal grounding.

