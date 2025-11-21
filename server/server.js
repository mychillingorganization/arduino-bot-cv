// server/server.js
const express = require('express');
const app = express();
const http = require('http').createServer(app);
const { SerialPort } = require('serialport');
const cors = require('cors');

// Allow React (running on port 5173) to connect
const io = require('socket.io')(http, {
    cors: {
        origin: "*", // Allow all origins for development
        methods: ["GET", "POST"]
    }
});

// === CONFIGURATION ===
const PORT_NAME = "COM5"; // CHANGE THIS TO YOUR ARDUINO PORT
const BAUD_RATE = 9600;

// Initialize Serial Port
const port = new SerialPort({ path: PORT_NAME, baudRate: BAUD_RATE });

port.on('open', () => {
    console.log('Arduino Serial Port Opened');
});

port.on('error', (err) => {
    console.error('Error: ', err.message);
});

io.on('connection', (socket) => {
    console.log('React Client connected');

    // Handle Car Movement
    socket.on('car_move', (command) => {
        console.log('Car CMD:', command);
        port.write(command);
    });

    // Handle Arm Movement
    socket.on('arm_data', (data) => {
        // Format: "C:90,90,90,0\n"
        const commandString = `C:${data.b},${data.s},${data.e},${data.g}\n`;
        port.write(commandString, (err) => {
            if (err) console.log('Serial Write Error:', err.message);
        });
    });
});

http.listen(3000, () => {
    console.log('Backend running on http://localhost:3000');
});