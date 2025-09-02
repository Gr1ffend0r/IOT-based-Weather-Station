# 🌦️ IoT-based Weather Station

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![MQTT](https://img.shields.io/badge/MQTT-3.1.1-brightgreen)](http://mqtt.org/)

A comprehensive IoT-based weather monitoring system that collects and displays real-time environmental data including temperature, humidity, O2 levels, atmospheric pressure, solar radiation, and UV intensity.

## 🌟 Features

- **Multi-sensor Integration**:
  - Temperature monitoring (MPC9700A)
  - Humidity sensing (HIH 5030)
  - O2 level detection
  - Atmospheric pressure measurement
  - Solar radiation monitoring
  - UV intensity detection (ML8511)

- **Interactive Display**:
  - Custom LCD interface
  - Multiple display screens
  - Touch controls for navigation

- **IoT Connectivity**:
  - MQTT protocol for remote monitoring
  - Real-time data transmission
  - Web interface for remote access

- **Data Logging**:
  - Local data storage
  - Historical data visualization
  - Export capabilities

## 🛠️ Hardware Requirements

- Arduino board (ATmega-based)
- HIH 5030 Humidity Sensor
- MPC9700A Temperature Sensor
- ML8511 UV Sensor
- O2 Sensor
- Atmospheric Pressure Sensor
- Solar Radiation Sensor
- TFT LCD Display with Touch
- MQTT-capable WiFi/Network Module

## 🚀 Getting Started

### Prerequisites

- Arduino IDE
- Required libraries:
  - PubSubClient (for MQTT)
  - Adafruit_GFX (for display)
  - Adafruit_TFTLCD (for TFT display)

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/IOT-based-Weather-Station.git
   cd IOT-based-Weather-Station
   ```

2. Open `Weather.ino` in Arduino IDE

3. Install required libraries through Arduino Library Manager

4. Configure MQTT settings in the code

5. Upload the sketch to your Arduino board

## 📡 MQTT Integration

The system uses MQTT for remote monitoring. The included Python scripts provide:

- `mqtt-sender.py`: Publishes sensor data to MQTT broker
- `mqtt-receiver.py`: Subscribes to MQTT topics and logs data
- `mqtt-receiver-ws.py`: WebSocket server for real-time web interface

### MQTT Topics
- `weather/temperature`
- `weather/humidity`
- `weather/o2`
- `weather/pressure`
- `weather/solar`
- `weather/uv`

## 🖥️ Display Interface

The LCD interface cycles through different screens showing:
1. Temperature & Humidity
2. O2 Levels
3. Atmospheric Pressure
4. Solar Radiation
5. UV Intensity

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with ❤️ using Arduino
- Thanks to all open-source libraries and tools used in this project

---

🌤️ *Stay informed about your environment with real-time weather data!*
