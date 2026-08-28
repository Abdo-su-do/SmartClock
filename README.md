# 🕒 ATmega32 Smart Clock

A feature-rich, embedded C Smart Clock built for the ATmega32 microcontroller. This project goes beyond just telling time—it serves as a desk companion packed with productivity tools, environmental sensors, and interactive menus, all controlled via a matrix keypad and displayed on a crisp character LCD.

---

## ✨ Features

- **Real-Time Clock (RTC):** Accurate time and date tracking powered by a DS3231 module via I2C. Supports both 12-hour (AM/PM) and 24-hour formats.
- **Environmental Monitoring:** Built-in temperature and humidity tracking (via ADC) with selectable units (Celsius/Fahrenheit).
- ** Alarm:** Configure alarm with a built-in buzzer system. Includes options to Snooze or Turn Off.
- **Productivity Suite:**
  - **Pomodoro Tracker:** Dedicated work, short break, and long break countdowns with audible buzzer notifications. Includes an interactive settings menu to customize session lengths.
  - **Stopwatch:** Standard stopwatch functionality for tracking elapsed time.
  - **Countdown Timer:** Configurable timer with an alarm when the time runs out.
- **Interactive UI:** A multi-screen LCD interface controlled by a matrix keypad, featuring a dedicated Settings menu to configure time, date, alarms, and display preferences.

## 🛠 Hardware Components

To build or simulate this project, you will need the following hardware components:
- **Microcontroller:** ATmega32 (8MHz / 16MHz)
- **Display:** 20x4 Character LCD (HD44780 compatible)
- **Input:** 4x4 Matrix Keypad
- **RTC Module:** DS3231 (I2C interface)
- **Sensors:** LM35 (or similar analog temp sensor connected to ADC)
- **Output:** Buzzer (Connected to PORTC, PIN2)
- **Simulation:** Proteus 8 Professional (Netlist & project files included)

## 🗂 Software Architecture

The software is structured using a standard layered automotive/embedded architecture for high modularity and reusability:

- **MCAL (Microcontroller Abstraction Layer):** 
  - `DIO` (Digital I/O)
  - `TIMER` (Timer0 for intervals, interrupts)
  - `I2C` (Two-Wire Interface for RTC)
  - `ADC` (Analog-to-Digital Converter for sensors)
  - `EXTI` & `GI` (External and Global Interrupts)
- **HAL (Hardware Abstraction Layer):**
  - `LCD` (Driver for display rendering)
  - `KEYPAD` (Driver for reading matrix input)
  - `RTC` (Driver for DS3231 I2C communication)
- **APP Layer:**
  - `main.c` (Screen state machine and main event loop)
  - `Timer.c` / `TIMER_APP` (Background timekeeping & buzzer handling)
  - `POMODORRO.c` (Pomodoro logic and state management)
  - `SET.c` (Settings menu logic)

## 🎮 Navigation & Controls

The system utilizes a matrix keypad for navigation. The primary mapping is as follows:

| Key | Action | Description |
| :---: | :--- | :--- |
| **`+`** | Next Screen | Cycles forward through the available screens (Time, Temp, Alarm, Timer, etc.) |
| **`-`** | Previous Screen | Cycles backward through the screens |
| **`*`** | Settings / Edit | Enters the Settings menu or edits the current configuration (e.g., inside Pomodoro) |
| **`/`** | Start / Pause / OK | Starts/Pauses the Stopwatch or Pomodoro, or confirms an action |
| **`=`** | Back / Reset | Resets the current timer or exits a menu |
| **`0-9`** | Numeric Input | Used to type in time/date values and configure alarms |

### Screen Flow
1. **Time & Date Screen** (Default)
2. **Temperature & Humidity Screen**
3. **Alarms Screen**
4. **Timer Screen**
5. **Stopwatch Screen**
6. **Pomodoro Screen**

*(Note: Pressing `*` anywhere usually brings you to the global **Settings Screen**, except within the Pomodoro screen where it opens the specific Pomodoro configurations).*

## 🚀 Getting Started

### Compilation
This project uses the AVR-GCC toolchain. 
1. Open the project in your preferred IDE (e.g., Microchip Studio, Eclipse AVR) or use the included `Makefile`.
2. Compile the source files into a `.hex` file.
```bash
make -C Debug
```

### Simulation
If you don't have the physical hardware, you can run the project in Proteus:
1. Open the `.pdsprj` Proteus file located in the project directory.
2. Double-click the ATmega32 MCU in the schematic.
3. Set the **Program File** to the compiled `.hex` file.
4. Run the simulation and interact with the keypad!

## 📜 License
This project is open-source. Feel free to fork, modify, and integrate these drivers into your own AVR projects!
