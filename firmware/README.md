## Firmware

### GPIO Assignments

#### Servos
- **Right Servo:** GPIO **13**
- **Left Servo:** GPIO **27**

#### Motors
- **Right Motor 1:** GPIO **25**
- **Right Motor 2:** GPIO **26**
- **Left Motor 1:** GPIO **32**
- **Left Motor 2:** GPIO **33**

#### Hall Sensors
- **Right Hall Sensor:** GPIO **34**
- **Left Hall Sensor:** GPIO **35**

#### Emergency Stop
- **Emergency Stop:** GPIO **18**

## Web App

The application in `/data` consists of several key files:

- `index.html`: Main structure of the web app
- `main.js`: Core functionality and WebSocket communication
- `persistent.js`: Handles data logging and export features
- `modal.js`: Manages modal dialogs
- `style.css`: Styles for the interface

To modify the application, edit these files and update them on the ESP32.