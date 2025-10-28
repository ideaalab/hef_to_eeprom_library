# HEF to EEPROM Library for CCS Compiler

## 📌 Author
**Martin Andersen**  
[IDEAA Lab](http://www.ideaalab.com)

---

## 📖 What does this library do?

This library allows you to use the **High Endurance Flash (HEF)** of **PICs without internal EEPROM**, emulating the standard `read_eeprom()` and `write_eeprom()` functions.

Since HEF is part of the **program memory**, its structure differs from EEPROM:
- Each storage unit is a **word** of 12 or 14 bits (depending on the PIC).
- To avoid data loss, **only the low byte (LSB) is used** for byte operations.
- HEF provides **100,000 erase/write cycles** endurance (compared to 1,000,000 for traditional EEPROM).

---

## ✨ Features

- ✅ **Transparent EEPROM emulation** - Drop-in replacement for standard EEPROM functions
- ✅ **Auto-detection** - Automatically detects HEF address for 38+ supported PICs
- ✅ **Byte, Word & Block operations** - Read/write single bytes, words, or blocks
- ✅ **Interrupt protection** - Automatically disables interrupts during write operations
- ✅ **No external dependencies** - Works with CCS C Compiler built-in functions
- ✅ **Write optimization** - Skips writes when value already stored (extends HEF lifespan)

---

## 🎯 Supported Microcontrollers

### PIC10 Family (12-bit words)
- PIC10F320, PIC10F322

### PIC12 Family (14-bit words)
- PIC12F1501, PIC12F1571, PIC12F1572, PIC12F1612

### PIC16F Family (14-bit words)
**145x series:** 16F1454, 16F1455, 16F1459  
**150x series:** 16F1501, 16F1503, 16F1507, 16F1508, 16F1509  
**151x series:** 16F1512, 16F1513, 16F1516, 16F1517, 16F1518, 16F1519  
**152x series:** 16F1526, 16F1527  
**161x series:** 16F1615, 16F1619  
**170x series:** 16F1703, 16F1704, 16F1707, 16F1708, 16F1713  
**171x series:** 16F1716, 16F1717, 16F1718, 16F1719

---

## ⚙️ Installation

1. Copy `hef.h` and `hef.c` to your project folder
2. Include the library in your main file:
   ```c
   #include "hef.c"  // CCS C Compiler requires including .c file
   ```
3. The library will automatically detect your PIC and configure HEF

### Manual Configuration (Optional)

If using an unsupported PIC, manually define the HEF start address:
```c
#define HEF_START_ADDRESS 0x1F80  // Example for 8K Flash PICs
#include "hef.c"
```

### Reserving HEF Memory

**Recommended:** Add this to your **Project Properties → Linker → Additional Options**:
```
--ROM=default,-1F80-1FFF    (for PIC16F1455 and similar 8K PICs)
--ROM=default,-3F80-3FFF    (for 16K PICs like PIC16F1518/19)
```

The library also includes an `#org` directive for automatic reservation, but linker options are more reliable.

---

## 📚 API Reference

### Basic Functions (EEPROM Compatible)

```c
// Write a byte to HEF
write_eeprom(address, value);

// Read a byte from HEF
byte data = read_eeprom(address);
```

### Word Operations (12 or 14 bits)

```c
// Write a word (full 12/14 bits preserved)
write_eeprom_word(address, word_value);

// Read a word (12/14 bits masked automatically)
int16 word_data = read_eeprom_word(address);
```

### Block Operations - Bytes

```c
// Write multiple bytes
byte buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
write_eeprom_block(address, buffer, 10);

// Read multiple bytes
byte buffer[10];
read_eeprom_block(address, buffer, 10);
```

### Block Operations - Words

```c
// Write multiple words
int16 word_buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
write_eeprom_block_word(address, word_buffer, 10);

// Read multiple words
int16 word_buffer[10];
read_eeprom_block_word(address, word_buffer, 10);
```

### Direct HEF Functions

All EEPROM-compatible macros map to these functions:

```c
// Single operations
hef_read_byte(addr);                      // Read single byte
hef_read_word(addr);                      // Read single word
hef_write_byte(addr, value);              // Write single byte
hef_write_word(addr, value);              // Write single word

// Block operations
hef_read_block(addr, ptr, count);         // Read multiple bytes
hef_write_block(addr, ptr, count);        // Write multiple bytes
hef_read_block_word(addr, ptr, count);    // Read multiple words
hef_write_block_word(addr, ptr, count);   // Write multiple words
```

---

## 💡 Usage Examples

A complete working example is available in `/demo/main.c`

### Example 1: Basic EEPROM Emulation
```c
#include <16F1455.h>
#include "hef.c"

void main() {
    // Write some data
    write_eeprom(0, 0x55);
    write_eeprom(1, 0xAA);
    
    // Read it back
    byte value1 = read_eeprom(0);  // Returns 0x55
    byte value2 = read_eeprom(1);  // Returns 0xAA
    
    while(1);
}
```

### Example 2: Storing Configuration Data
```c
#include <16F1455.h>
#include "hef.c"

#define CONFIG_ADDRESS 0

typedef struct {
    byte device_id;
    byte firmware_version;
    int16 serial_number;
    byte checksum;
} Config;

void save_config(Config *cfg) {
    write_eeprom_block(CONFIG_ADDRESS, (byte*)cfg, sizeof(Config));
}

void load_config(Config *cfg) {
    read_eeprom_block(CONFIG_ADDRESS, (byte*)cfg, sizeof(Config));
}

void main() {
    Config my_config;
    
    // Save configuration
    my_config.device_id = 42;
    my_config.firmware_version = 1;
    my_config.serial_number = 12345;
    my_config.checksum = 0xAB;
    save_config(&my_config);
    
    // Load configuration after power cycle
    Config loaded_config;
    load_config(&loaded_config);
    
    // Verify data
    if(loaded_config.device_id == 42) {
        // Configuration loaded successfully
    }
    
    while(1);
}
```

### Example 3: Data Logging with Words
```c
#include <16F1619.h>
#include "hef.c"

#define LOG_START 0
#define MAX_LOGS 64  // 128 bytes / 2 bytes per log

void log_temperature(int16 temp) {
    static byte log_index = 0;
    
    if (log_index < MAX_LOGS) {
        write_eeprom_word(LOG_START + log_index, temp);
        log_index++;
    }
}

void main() {
    // Log temperature readings (in 0.1°C units)
    log_temperature(250);  // 25.0°C
    log_temperature(312);  // 31.2°C
    log_temperature(189);  // 18.9°C
    
    // Read back logs
    int16 temps[3];
    read_eeprom_block_word(LOG_START, temps, 3);
    
    // Process logged data
    for(int i = 0; i < 3; i++) {
        // temps[i] contains the logged values
    }
    
    while(1);
}
```

### Example 4: Block Word Operations
```c
#include <16F1455.h>
#include "hef.c"

void main() {
    // Prepare data buffer
    int16 sensor_data[8] = {1234, 5678, 9012, 3456, 7890, 1122, 3344, 5566};
    
    // Write entire block at once
    write_eeprom_block_word(0, sensor_data, 8);
    
    // Read back entire block
    int16 retrieved_data[8];
    read_eeprom_block_word(0, retrieved_data, 8);
    
    // All 8 words are now in retrieved_data[]
    
    while(1);
}
```

---

## ⚠️ Important Notes

### Flash Memory Write Behavior

PIC Flash memory can only change bits from `1` to `0` during a write operation. To change a `0` back to `1`, you must **erase the entire page**, which sets all bits to `1`.

**Example with 14-bit words:**
```
Stored value:    11111100000011
New value:       11110011111111
-------------------------------
Result (AND):    11110000000011
```

This means:
- Previous `0` bits remain `0`
- You can only write new `0` bits
- The library handles this automatically by checking if the value is already stored

### HEF Lifespan

| Memory Type | Erase/Write Cycles | Temperature Range |
|-------------|-------------------|-------------------|
| **HEF** | ~100,000 cycles | 0°C to 60°C |
| Regular Flash | ~10,000 cycles | Full range |
| Traditional EEPROM | ~1,000,000 cycles | Full range |

**Important:** The library optimizes HEF lifespan by skipping writes when the value is already stored. This can extend the practical lifetime significantly.

### Memory Organization

- All supported PICs have **128 bytes (128 words)** of HEF memory
- Each word stores:
  - **12 bits** for PIC10F (baseline architecture)
  - **14 bits** for PIC12F/16F (enhanced mid-range)
- Byte operations use only the **LSB (low 8 bits)**
- Word operations preserve all **12 or 14 bits**

### RAM Usage

- **Single operations:** No RAM buffer needed
- **Block operations (bytes):** Minimal temporary variables
- **Block operations (words):** Temporary buffer during read (2 × count bytes)

---

## 🔧 How It Works

1. **Auto-Detection:** Library detects your PIC model using `getenv("DEVICE")`
2. **HEF Mapping:** Automatically maps HEF region to correct address
3. **Memory Reservation:** Uses `#org` directive to prevent compiler overwriting HEF
4. **Byte Operations:** Reads/writes LSB of each program memory word
5. **Word Operations:** Reads/writes full 12/14-bit words with proper masking
6. **Write Optimization:** 
   - Checks if value already stored
   - Skips write if identical (extends HEF lifespan)
   - Disables interrupts during write
   - Re-enables interrupts after write

---

## 🛠️ Troubleshooting

### "HEF_START_ADDRESS not defined" Error
**Cause:** Your PIC is not in the auto-detection list  
**Solution:** Manually define `HEF_START_ADDRESS` before including the library:
```c
#define HEF_START_ADDRESS 0x1F80
#include "hef.c"
```

### Data Gets Overwritten by Program Code
**Cause:** HEF memory not reserved in linker  
**Solution:** Add linker option (see Installation section) or verify `#org` directive is working

### Values Don't Persist After Power Cycle
**Possible causes:**
- Running in debug mode (some debuggers don't preserve HEF)
- HEF not properly reserved
- Writing to wrong addresses

**Solutions:**
- Test in release mode without debugger
- Verify HEF reservation in linker settings
- Check that addresses are within 0 to 127

### "Internal EEPROM is being used" Warning
**Cause:** Your PIC has real EEPROM  
**Behavior:** Library automatically uses native EEPROM functions  
**Action:** This is normal, no action needed

### Compilation Errors with `hef.c`
**Cause:** Path issues with `#include "hef.c"`  
**Solution:** 
- Ensure `hef.c` and `hef.h` are in the same folder
- Use correct relative path (e.g., `"../hef.c"` if in subfolder)
- Or add both files to project and include only `hef.h`

---

## 📂 Repository Structure

```
hef_to_eeprom_library/
├── hef.h              # Library header file
├── hef.c              # Library implementation
├── README.md          # This file
└── demo/
    └── main.c         # Complete working example
```

---

## 🔄 Version History

**Current Version:** 1.0
- Initial release
- Support for 38+ PIC microcontrollers
- Byte, word, and block operations
- Automatic PIC detection
- Write optimization for extended lifespan

---

## 📜 License
This project is licensed under the **MIT License**.

---

## 🤝 Contributing

🚀 **Contributions and improvements are welcome!**

To contribute:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

**Ideas for contributions:**
- Add support for more PIC models
- Optimize block operations
- Add wear-leveling algorithms
- Create additional examples
- Improve documentation

---

## 📧 Contact

For questions, suggestions, or issues:
- **Author:** Martin Andersen
- **Website:** [IDEAA Lab](http://www.ideaalab.com)
- **GitHub:** Open an issue in the repository

---

🎯 **Thank you for using this library!**
