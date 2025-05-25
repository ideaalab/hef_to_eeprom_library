# Librería para Emular EEPROM con HEF en PICs sin EEPROM Interna

## 📌 Autor
**Martin Andersen**  
[IDEAA Lab](http://www.ideaalab.com)

---

## 📖 ¿Qué hace esta librería?

Esta librería permite usar la **High Endurance Flash (HEF)** de los **PICs que no tienen EEPROM interna**, simulando las funciones estándar `read_eeprom()` y `write_eeprom()`.

Dado que la HEF es una parte de la **memoria de programa**, su estructura es diferente a la EEPROM:
- Cada unidad de información es una **"palabra"** de 13 o 15 bits (según el PIC).
- Para evitar perder datos, **solo se usa el byte bajo (LSB)**.
- Si se usa un buffer, **los bytes impares no se utilizan**; solo almacenamos en los pares.

---

## ⚠ Precauciones

1. **Memoria RAM suficiente:**  
   - Para conservar los datos previos en la HEF, se necesita un buffer RAM del tamaño de la página.
   - Si la RAM es limitada, se puede configurar la librería para sobrescribir sin conservar datos anteriores.

2. **Ciclo de vida de la HEF:**  
   - La HEF tiene **una vida útil limitada** que se consume con los **borrados de página**, no con las escrituras.

3. **El compilador podría usar la HEF:**  
   - La HEF está dentro de la **memoria de programa**, por lo que si el código es demasiado grande, **el compilador podría sobrescribirla**.
   - Se reserva el segmento de HEF usando directiva #org, pero no se ha probado.

---

## ⚙ Configuración de la librería

Para que la librería funcione correctamente, es necesario definir los parámetros específicos de la HEF para el **PIC utilizado**:

```c
#define HEF_START_ADDRESS   0xF000  // Dirección de inicio de HEF
#define HEF_SIZE            128     // Tamaño de la HEF en bytes
```

Se han creado defines para los principales PICs de la familia 10/12/16, por lo que se puede definir `HEF_START_ADDRESS` con estos valores:

```c
#define HEF_START_ADDRESS   HEF_16F1455
```

### 📌 Modo de escritura

Existen dos modos de escritura en la HEF. Debes definir **uno de los siguientes**:

1. **HEF_LOSS_WRITE** → **No conserva los datos anteriores** (NO requiere buffer en RAM).
   ```c
   #define HEF_LOSS_WRITE  // Pierde el contenido de HEF cuando escribe
   ```

2. **HEF_KEEP_WRITE** → **Conserva los datos anteriores** (Requiere buffer en RAM).
   ```c
   #define HEF_KEEP_WRITE  // Mantiene el contenido de HEF cuando escribe
   ```

Por **defecto**, la librería mantiene los datos en la HEF (`HEF_KEEP_WRITE`).

---

## 📌 Funciones disponibles

### `write_eeprom(int8 address, int data)`
Escribe un byte en la HEF.

#### Parámetros:
- **`address`**: Dirección dentro de la HEF _(0 - HEF_SIZE)_.
- **`data`**: Valor a escribir _(0 - 255)_.

Ejemplo de uso:
```c
write_eeprom(10, 0x55);  // Escribe el valor 0x55 en la dirección 10 de HEF
```

---

## 🛠 Funcionamiento del Borrado y Escritura en Memoria Flash de los PIC

En los microcontroladores PIC, la memoria Flash solo permite cambiar bits de `1` a `0` 
durante la escritura. Para volver a poner un `0` en `1`, es necesario borrar la página 
completa, lo que restablece todos los bits a `1`.

### ⚙ Comportamiento de la Escritura sin Borrado Previo

Si se escribe en una posición de memoria sin haberla borrado antes, el valor resultante 
será la combinación del valor previo y el nuevo, aplicando una operación **AND bit a bit**.

Ejemplo con palabras de 14 bits:

```
Valor almacenado:    11111100000011
Nuevo valor escrito: 11110011111111
-------------------------------
Valor final:         11110000000011
```

Esto significa que los `0` previos se mantienen y solo se pueden escribir nuevos `0`. 
Para garantizar que un valor se almacene correctamente, **es necesario borrar la página antes de escribir**.

---

## 📜 Licencia
Este proyecto está bajo la licencia **MIT**.

---

🚀 **Contribuciones y mejoras son bienvenidas.** ¡No dudes en hacer un fork y proponer cambios! 🎯
