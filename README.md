# G29_CustomSteeringWheel

A custom ESP8266-based steering wheel controller for Logitech G29 racing wheel simulation. This project enables wireless control of PS4 button inputs and displays timing information on a 6-digit LED display.

## Features

- **Wireless Communication**: Uses ESP-NOW protocol for reliable peer-to-peer communication
- **PS4 Button Control**: Emulates PS4 Circle button presses via wireless remote
- **Timer Display**: Shows elapsed time on 6-digit TM1637 display when button is held >5 seconds
- **Visual Feedback**: Status LED indicates button state and wireless communication
- **Smart Reset**: Triple-click remote button to reset timer
- **PlatformIO Compatible**: Built for embedded development with PlatformIO

## Hardware Requirements

- **MCU**: ESP8266 (NodeMCU v2 or compatible)
- **Display**: 6-digit TM1637 LED display module
- **Remote MCU**: Another ESP8266 device running the wireless transmitter code
- **Power Supply**: 5V power source for ESP8266 and display

## Wiring Configuration

| Component | ESP8266 GPIO | Description |
|-----------|-------------|-------------|
| TM1637 CLK | GPIO 12 (D6) | Display clock signal |
| TM1637 DIO | GPIO 14 (D5) | Display data signal |
| Status LED | GPIO 4 (D2) | Visual feedback indicator |
| PS4 Button | GPIO 5 (D1) | Output to PS4 button circuit |

### Power Connections
- ESP8266 VCC → 5V
- ESP8266 GND → GND
- TM1637 VCC → 5V
- TM1637 GND → GND

## Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP8266 development board support installed

### Setup Steps
1. Clone this repository
2. Open the project in your preferred IDE (VSCode with PlatformIO recommended)
3. Install dependencies:
   ```bash
   pio lib install
   ```
4. Build and upload:
   ```bash
   pio run --target upload
   ```
5. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Usage

### Basic Operation
1. **Power on** both the steering wheel MCU and remote transmitter
2. **Press remote button** - PS4 Circle button will be activated
3. **Release remote button** - PS4 Circle button will be deactivated
4. **Hold button >5 seconds** - Timer starts and displays on LED screen
5. **Triple-click remote** - Resets timer and stops display

### Timer Feature
- When remote button is held for more than 5 seconds, a timer automatically starts
- The 6-digit display shows elapsed time in HH:MM:SS.mmm format
- Timer continues running until manually reset
- Reset by pressing remote button 3 times quickly (within 333ms intervals)

### LED Status Indicators
- **LED ON**: Remote button pressed, PS4 button active
- **LED OFF**: Remote button released, PS4 button inactive
- **LED blinks with button state**: Indicates wireless communication active

## Dependencies

- **Arduino Framework**: Core ESP8266 functionality
- **ESP8266WiFi**: WiFi connectivity for ESP-NOW
- **espnow**: Wireless peer-to-peer communication
- **TM1637_6D**: 6-digit LED display driver

### Library Installation
The TM1637_6D library is included as a git submodule. To initialize:
```bash
git submodule update --init --recursive
```

## Configuration

### Pin Configuration
All pin assignments are defined as constants in `src/main.cpp`:
```cpp
const int8_t CLK_Display = 12;
const int8_t DIO_Display = 14;
const int8_t Status_LED = 4;
const int8_t PS4_Button = 5;
```

### Timing Parameters
- **Timer trigger**: 5000ms (5 seconds)
- **Triple-click window**: 333ms
- **Display update rate**: 10ms

## Troubleshooting

### Common Issues

**Display not working:**
- Check wiring connections (CLK and DIO)
- Verify 5V power supply
- Ensure display module is compatible with TM1637 protocol

**No wireless communication:**
- Verify both devices are on same WiFi channel
- Check ESP-NOW initialization in serial monitor
- Ensure transmitter code is running on remote device

**Timer not starting:**
- Hold button for full 5 seconds
- Check serial output for timing debug information
- Verify button state changes are being received

**PS4 not responding:**
- Check output circuit connected to GPIO 5
- Verify button state is changing (use LED indicator)
- Ensure proper pull-up/pull-down resistors if needed

### Debug Information
Enable serial monitor at 115200 baud to see:
- Packet reception confirmations
- Button state changes
- Timer activation messages
- Error messages from ESP-NOW initialization

## Development

### Project Structure
```
G29_CustomSteeringWheel/
├── src/
│   └── main.cpp          # Main application code
├── lib/
│   └── TM1637_6D-master/ # Display library
├── include/              # Header files (empty)
├── test/                 # Unit tests (placeholder)
└── platformio.ini        # PlatformIO configuration
```

### Adding Features
- Modify `src/main.cpp` for logic changes
- Add new display functions using TM1637_6D library
- Extend wireless protocol in the `struct_message` structure

### Testing
- Use PlatformIO unit testing framework
- Test wireless communication with mock transmitter
- Verify timing accuracy with serial output

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the terms specified in the [License](License) file.

## Related Projects

- G29_Handbrake - Wireless handbrake controller
- G29_ShifterDisplay - Gear shift indicator
- G29_ShifterReceiver - Wireless shifter receiver
- G29_ShifterSniffer - CAN bus protocol analyzer

## Support

For issues and questions:
1. Check the troubleshooting section above
2. Review serial monitor output for error messages
3. Verify hardware connections match the wiring diagram
4. Ensure both transmitter and receiver code are compatible

## Version History

- v1.0.0 - Initial release with basic functionality
- Timer display with smart reset
- ESP-NOW wireless communication
- PS4 button emulation
