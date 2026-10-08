# Snake ESP32 OLED

Juego Snake realizado con ESP32, pantalla OLED 128x64 y joystick analógico.

## Componentes
- ESP32
- OLED 128x64 SSD1306 I2C
- Joystick analógico con botón
- Cables Dupont

## Conexiones
### OLED
- GND -> GND
- VCC -> 3V3
- SCL -> GPIO 22
- SDA -> GPIO 21

### Joystick
- GND -> GND
- +5V -> 3V3
- VRx -> GPIO 34
- VRy -> GPIO 35
- SW -> GPIO 25

## Librerías
- Adafruit GFX Library
- Adafruit SSD1306

## Funcionamiento
El joystick controla las cuatro direcciones de la serpiente. La comida aparece aleatoriamente y cada comida aumenta la puntuación y la longitud. La velocidad aumenta progresivamente.

El juego termina al tocar una pared o el propio cuerpo. Pulsar el joystick inicia el juego y permite reiniciarlo después de un Game Over.
