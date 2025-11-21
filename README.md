# 🤖 Robot AI Controller

A real-time computer vision-controlled robotic arm and car system using React, Node.js, Arduino, and MediaPipe hand tracking.

## 📋 Overview

This project combines web technologies with hardware control to create an interactive robot system that can be controlled through:
- **Hand Gesture Recognition** - Control a 4-DOF robotic arm using MediaPipe hand tracking
- **Manual Controls** - Web-based D-pad interface for car movement

The system captures hand movements via webcam, processes them in real-time using MediaPipe Hands, and sends servo angle commands to an Arduino-controlled robotic arm through WebSocket communication.

## 🎯 Features

- **Real-time Hand Tracking**: MediaPipe-powered computer vision with visual feedback
- **Robotic Arm Control**: 4 degrees of freedom (Base, Shoulder, Elbow, Gripper)
- **Car Movement**: Forward, backward, left, right controls
- **WebSocket Communication**: Low-latency control via Socket.IO
- **Responsive UI**: Built with React and Vite for fast development
- **Throttled Commands**: 500ms delay prevents overwhelming the hardware

## 🏗️ Architecture

```
┌─────────────┐      WebSocket      ┌─────────────┐      Serial      ┌──────────┐
│   React     │ ◄─────────────────► │   Node.js   │ ◄───────────────► │ Arduino  │
│   Client    │   (Socket.IO)       │   Server    │   (SerialPort)   │ Hardware │
│ (Port 5173) │                     │ (Port 3000) │                  │          │
└─────────────┘                     └─────────────┘                  └──────────┘
      │                                                                     │
      │                                                                     │
  Webcam Input                                                    Servos + Motors
  MediaPipe CV
```

## 📦 Project Structure

```
robot_controller/
├── arduino_code/
│   └── robot_sketch/
│       └── robot_sketch.ino          # Arduino firmware
├── client/                            # React frontend
│   ├── src/
│   │   ├── components/
│   │   │   ├── CarControls.jsx       # D-pad car controls
│   │   │   └── VisionControl.jsx     # Hand tracking & arm control
│   │   ├── socket.js                 # WebSocket client config
│   │   └── App.jsx                   # Main app component
│   └── package.json
├── server/
│   ├── server.js                     # Node.js WebSocket & Serial bridge
│   └── package.json
└── README.md
```

## 🚀 Getting Started

### Prerequisites

- **Node.js** (v16 or higher)
- **Arduino IDE** (for uploading firmware)
- **Arduino Board** (tested with Arduino Uno/Mega)
- **Hardware**:
  - PCA9685 PWM Servo Driver
  - 4x Servo motors (robotic arm)
  - L298N Motor Driver
  - DC motors (car)
  - Webcam

### Installation

#### 1. Arduino Setup

1. Open `arduino_code/robot_sketch/robot_sketch.ino` in Arduino IDE
2. Install required library: `Adafruit_PWMServoDriver`
3. Update the serial port in `server/server.js`:
   ```javascript
   const PORT_NAME = "COM5"; // Change to your Arduino port (COM3, /dev/ttyUSB0, etc.)
   ```
4. Upload the sketch to your Arduino

#### 2. Backend Setup

```bash
cd server
npm install
node server.js
```

The server will start on `http://localhost:3000`

#### 3. Frontend Setup

```bash
cd client
npm install
npm run dev
```

The React app will start on `http://localhost:5173`

### First Run

1. Connect Arduino via USB
2. Start the Node.js server (it will establish serial connection)
3. Start the React dev server
4. Open `http://localhost:5173` in your browser
5. Allow webcam access when prompted

## 🎮 How to Use

### Hand Tracking Mode

1. Click **"START TRACKING (CV)"** button
2. Position your hand in front of the webcam
3. The system tracks your hand and maps movements to servo angles:
   - **Base (Rotation)**: Wrist horizontal position (X-axis)
   - **Shoulder**: Wrist vertical position (Y-axis)
   - **Elbow**: Height difference between wrist and index finger tip
   - **Gripper**: Distance between thumb tip and index finger tip
4. Visual feedback shows:
   - Green skeleton overlay on your hand
   - Real-time angle values (Base, Shoulder, Elbow, Grip)
5. Commands are sent every 500ms to prevent overwhelming the arm motors

### Manual Car Controls

Use the D-pad buttons to control car movement:
- **▲** - Forward
- **▼** - Backward
- **◄** - Turn Left
- **►** - Turn Right
- **STOP** - Emergency stop

Release any button to automatically stop the car.

## 🔧 Configuration

### Adjust Hand Tracking Sensitivity

In `client/src/components/VisionControl.jsx`:

```javascript
hands.setOptions({
  maxNumHands: 1,
  modelComplexity: 1,
  minDetectionConfidence: 0.5,  // Increase for less false positives
  minTrackingConfidence: 0.5,   // Increase for smoother tracking
});
```

### Change Command Throttle Rate

```javascript
// Current: 500ms delay between commands
if (now - lastSentRef.current > 500) {  // Adjust this value (in milliseconds)
  socket.emit('arm_data', { b, s, e, g });
  lastSentRef.current = now;
}
```

### Servo Angle Constraints

In `arduino_code/robot_sketch/robot_sketch.ino`:

```cpp
void updateAllServos() {
  baseAngle = constrain(baseAngle, 0, 180);
  shoulderAngle = constrain(shoulderAngle, 15, 160);  // Adjust min/max
  elbowAngle = constrain(elbowAngle, 0, 180);
  // ...
}
```

## 🐛 Troubleshooting

### Nothing displays at localhost:5173
- Ensure dependencies are installed: `npm install`
- Check React version compatibility (should be 18.x)
- Verify Vite is running without errors

### Serial port connection failed
- Check Arduino is connected and port name is correct in `server.js`
- Ensure Arduino IDE Serial Monitor is closed
- On Linux: Add user to dialout group: `sudo usermod -a -G dialout $USER`

### Hand tracking not working
- Allow webcam permissions in browser
- Ensure adequate lighting
- Check browser console for MediaPipe errors
- Try a different browser (Chrome/Edge recommended)

### Arm moves too fast/jerky
- Increase throttle delay in `VisionControl.jsx` (currently 500ms)
- Add smoothing to servo movements in Arduino code
- Reduce servo speed in hardware configuration

## 📚 Technologies Used

- **Frontend**: React 18, Vite, MediaPipe Hands, Socket.IO Client
- **Backend**: Node.js, Express, Socket.IO, SerialPort
- **Hardware**: Arduino, PCA9685, L298N Motor Driver
- **Computer Vision**: MediaPipe Hands (Google)

## 📄 License

This project is open source and available for educational purposes.

## 🤝 Contributing

Contributions are welcome! Feel free to submit issues or pull requests.

## 📧 Contact

For questions or support, please open an issue in the repository.

---

**Made with ❤️ by mychillingorganization**