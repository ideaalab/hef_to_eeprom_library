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
- ✅ **Byte, Word & Block operations** - Read/write single bytes, words, or multiple bytes
- ✅ **Interrupt protection** - Automatically disables interrupts during write operations
- ✅ **No external dependencies** - Works with CCS C Compiler built-in functions

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
2. Include the header in your main file:
```c
   #include "hef.h"
```
3. The library will automatically detect your PIC and configure HEF

### Manual Configuration (Optional)

If using an unsupported PIC, manually define the HEF start address:
```c
#define HEF_START_ADDRESS 0x1F80  // Example for 8K Flash PICs
#include "hef.h"
```

### Reserving HEF Memory

Add this to your **Project Properties → XC8 Linker → Additional Options**:
```
--ROM=default,-1F80-1FFF    (for PIC16F1455 and similar 8K PICs)
--ROM=default,-3F80-3FFF    (for 16K PICs like PIC16F1518/19)
```

Or use the `#org` directive (already included in hef.h):
```c
#org HEF_START_ADDRESS, HEF_END_ADDRESS {}
```

---

## 📚 API Reference

### Basic Functions (EEPROM Compatible)
```c
// Write a byte to HEF
write_eeprom(address, value);

// Read a byte from HEF
byte data = read_eeprom(address);
```

### Extended Functions
```c
// Write a word (12 or 14 bits)
write_eeprom_word(address, word_value);

// Read a word (12 or 14 bits)
int16 word_data = read_eeprom_word(address);

// Write multiple bytes
byte buffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
write_eeprom_block(address, buffer, 10);

// Read multiple bytes
byte buffer[10];
read_eeprom_block(address, buffer, 10);
```

### Direct HEF Functions
```c
hef_read_byte(addr);                    // Read single byte
hef_read_word(addr);                    // Read single word
hef_write_byte(addr, value);            // Write single byte
hef_write_word(addr, value);            // Write single word
hef_read_block(addr, ptr, count);       // Read multiple bytes
hef_write_block(addr, ptr, count);      // Write multiple bytes
```

---

## 💡 Usage Examples

### Example 1: Basic EEPROM Emulation
```c
#include <16F1455.h>
#include "hef.h"

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
#include "hef.h"

#define CONFIG_ADDRESS 0

typedef struct {
    byte device_id;
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
    my_config.serial_number = 12345;
    my_config.checksum = 0xAB;
    save_config(&my_config);
    
    // Load configuration
    Config loaded_config;
    load_config(&loaded_config);
    
    while(1);
}
```

### Example 3: Data Logging
```c
#include <16F1619.h>
#include "hef.h"

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
    log_temperature(250);  // 25.0°C
    log_temperature(312);  // 31.2°C
    
    // Read back first log entry
    int16 first_temp = read_eeprom_word(LOG_START);
    
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

- HEF endurance: **~100,000 erase/write cycles** (at 0°C to 60°C)
- Regular Flash: **~10,000 erase/write cycles**
- Traditional EEPROM: **~1,000,000 erase/write cycles**

### Memory Size

All supported PICs have **128 bytes** of HEF memory.

### RAM Usage

- **Byte/Word operations:** No RAM buffer needed
- **Block operations:** Uses temporary variables only during the operation

---

## 🔧 How It Works

1. **Detection:** Library automatically detects your PIC model using `getenv("DEVICE")`
2. **HEF Mapping:** Maps the HEF region to the correct address for your PIC
3. **Memory Reservation:** Uses `#org` directive to prevent compiler from using HEF for code
4. **Read Operations:** Reads program memory words and extracts the LSB (byte operations)
5. **Write Operations:** 
   - Checks if value is already stored (skip write if identical)
   - Disables interrupts
   - Writes to program memory
   - Re-enables interrupts

---

## 🛠️ Troubleshooting

### "HEF_START_ADDRESS not defined" Error
- Your PIC is not in the auto-detection list
- Manually define `HEF_START_ADDRESS` before including `hef.h`

### Data Gets Overwritten by Program Code
- Add linker option to reserve HEF memory (see Installation section)
- Check that your program size doesn't exceed available Flash

### Values Don't Persist After Power Cycle
- Ensure you're not in debug mode (some debuggers don't preserve HEF)
- Verify HEF memory is properly reserved in linker settings

### "Internal EEPROM is being used" Warning
- Your PIC has real EEPROM, library will use native functions automatically
- This is normal behavior, no action needed

---

## 📜 License
This project is licensed under the **MIT License**.

---

🚀 **Contributions and improvements are welcome!** Feel free to fork and submit pull requests! 🎯