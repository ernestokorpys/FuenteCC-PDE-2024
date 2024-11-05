# Sistema de Control de Fuente de Alimentación Digital

Este repositorio contiene el código fuente para el sistema de control digital de una fuente de alimentación DC ajustable, desarrollado como proyecto final. El sistema permite un control preciso de tensión y corriente, con varios modos y funciones de seguridad integradas. Las características principales incluyen:

- **Modos:** Tensión constante, corriente constante y modo rampa.
- **Algoritmo de Control:** Implementa anti-windup para evitar acumulaciones de control en condiciones de sobrecorriente o sobretensión.
- **Interfaz de Usuario:** Pantalla y entrada mediante encoders para ajustes y retroalimentación.
- **Protecciones:** Mecanismos de protección contra sobrecorriente y sobretensión.
- **Librerías Utilizadas:** Este proyecto utiliza las siguientes librerías:
  - `<Wire.h>`
  - `<Adafruit_ADS1X15.h>`
  - `<Adafruit_MCP4725.h>`
  - `<Adafruit_GFX.h>`
  - `<Adafruit_SSD1306.h>`
  - `<Key.h>`
  - `<Keypad.h>`
  - `"display.h"`
  - `"controller.h"`
  - `"encoder.h"`
  - `"keyboard.h"`

## Requisitos

Asegúrate de tener instaladas las librerías mencionadas en tu entorno de desarrollo para compilar y ejecutar el proyecto correctamente.

## Uso

Este proyecto está destinado para uso educativo y experimental en el diseño e implementación de fuentes de alimentación controladas digitalmente, y ofrece las siguientes características:

1. **Modo de Tensión Constante** - Mantiene una tensión de salida fija.
2. **Modo de Corriente Constante** - Mantiene una corriente de salida constante.
3. **Modo Rampa** - Aumenta gradualmente la tensión de salida hasta un nivel objetivo.

### Instalación

1. Clona este repositorio en tu equipo local.
2. Instala las librerías requeridas.
3. Sube el código al microcontrolador.

--- 
# Digital Power Supply Control System

This repository contains the source code for the digital control system of a variable DC power supply, developed as a final project. The system enables precise control of voltage and current, with various modes and integrated safety features. Key functionalities include:

- **Modes:** Constant Voltage, Constant Current, and Ramp Mode.
- **Control Algorithm:** Features anti-windup to prevent control action buildup in overcurrent or overvoltage conditions.
- **User Interface:** Display and encoder-based input for seamless adjustments and feedback.
- **Safety Protections:** Overcurrent and overvoltage protection mechanisms.
- **Libraries Used:** This project utilizes the following libraries:
  - `<Wire.h>`
  - `<Adafruit_ADS1X15.h>`
  - `<Adafruit_MCP4725.h>`
  - `<Adafruit_GFX.h>`
  - `<Adafruit_SSD1306.h>`
  - `<Key.h>`
  - `<Keypad.h>`
  - `"display.h"`
  - `"controller.h"`
  - `"encoder.h"`
  - `"keyboard.h"`

## Requirements

Ensure you have the above libraries installed in your development environment to successfully compile and run this project.

## Usage

This project is intended for educational and experimental use in the design and implementation of digitally controlled power supplies. It provides the following features:

1. **Constant Voltage Mode** - Maintains a fixed output voltage.
2. **Constant Current Mode** - Keeps a consistent current output.
3. **Ramp Mode** - Gradually increases the output voltage to a target level.

### Installation

1. Clone this repository to your local machine.
2. Install the required libraries.
3. Upload the code to the microcontroller.

---
