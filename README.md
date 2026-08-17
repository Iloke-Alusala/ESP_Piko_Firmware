# 🕹️ Piko Smartwatch Firmware

Firmware for the **Piko Smartwatch**, an open-source ESP32-based wearable designed to detect physical activity types (like walking, running, or sprinting) using onboard accelerometer data and display the current state on a TFT screen with animated GIFs.

---

## 🧠 Overview

This project uses real-time acceleration data to:
- Determine the user's **motion state** (idle, walking, running, sprinting).
- Count steps based on filtered acceleration.
- Display a corresponding **animated GIF** of the mascot “Piko” on a 240x240 TFT display.

The firmware is optimized for the **ESP32-C6 Beetle** and supports modular expansion.

---

## 📁 Folder Structure
```
ESP_Piko_Firmware/
│
├── MainSketch.ino # Main Arduino sketch
├── PikoAccelerate.h/.cpp # Motion detection logic
├── piko_idle.h # GIF animation for idle state
├── piko_walk.h # GIF animation for walking
├── piko_jog.h # GIF animation for running
├── piko_sprint.h # GIF animation for sprinting
├── piko_sleep.h # Optional sleep state GIF
```


---

## 🚀 Features

- **Step Counting**: Uses acceleration peaks and thresholds to count steps.
- **Motion Classification**: Determines activity (idle, walking, running, sprinting) based on standard deviation and average acceleration.
- **TFT Display Output**: Dynamically displays a different Piko animation depending on the activity type.
- **Modular GIF Storage**: Animations are stored as `.h` header files in PROGMEM, making them memory-efficient.

---

## ⚙️ How It Works

### 🧮 Step Detection
- Raw acceleration vector magnitude is calculated using: a = sqrt(x² + y² + z²)
- Movement is classified based on defined thresholds (see below).
- Steps are counted when the acceleration crosses a threshold and the user is not already mid-step.

### 🔁 Motion State Thresholds
| State       | Average Acceleration (ave) | Std Dev (std) |
|-------------|-----------------------------|---------------|
| Idling      | `std <= EXERCISING_THRESHOLD` | —             |
| Walking     | `ave > WALKING_THRESHOLD`     | `std > EXERCISING_THRESHOLD` |
| Running     | `ave > RUNNING_THRESHOLD`     | `std > EXERCISING_THRESHOLD` |
| Sprinting   | `ave > SPRINTING_THRESHOLD`   | `std > EXERCISING_THRESHOLD` |

Values are defined in `PikoAccelerate.h`.

---

## 📦 Dependencies

Ensure the following libraries are installed via Arduino Library Manager or PlatformIO:
- `TFT_eSPI`
- `Wire.h` for I2C
- A compatible GIF decoder if modified for runtime decoding (this build uses `.h` format GIFs from [`image_to_c`](https://github.com/bitbank2/image_to_c))

---

## 📸 Screens & Animations

Each motion state corresponds to a custom 240x240 GIF of “Piko”:
- `piko_idle.h` – Piko chilling 💤
- `piko_walk.h` – Piko strolling 🚶
- `piko_jog.h` – Piko jogging 🏃
- `piko_sprint.h` – Piko dashing 🏃‍♂️💨
- `piko_sleep.h` – (Optional) Sleep state 😴

---

## 🔧 Configuration

- Adjust thresholds in `PikoAccelerate.h` to fine-tune step and motion detection:
```cpp
#define EXERCISING_THRESHOLD 5
#define WALKING_THRESHOLD 50
#define RUNNING_THRESHOLD 100
#define SPRINTING_THRESHOLD 700
```
🙌 Credits
Created by Iloke Alusala, Rafael Cardoso and Lulama Lingela with ❤️ as part of the Chevo Collective initiative.

Inspired by physics, open-source tech, and a touch of fun with Piko the mascot.

📜 License
MIT License. Animations are original and may be used in personal or educational projects with attribution.
