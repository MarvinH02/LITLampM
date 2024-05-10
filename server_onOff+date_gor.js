const express = require('express');
const { exec } = require('child_process');
const cors = require('cors');
const app = express();
const port = 3000;

app.use(cors());

const demoPath = '/home/g10/rpi-rgb-led-matrix/examples-api-use';
const datePath = '/home/g10/rpi-rgb-led-matrix/examples-api-use';

// Function to run the demo command
function runDemoCommand(command, res) {
    exec(command, { cwd: demoPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing command '${command}': ${error}`);
            return res.status(500).send(`Error: ${error.message}`);
        }
        res.send(`Command executed successfully: ${stdout}`);
    });
}

// Function to start the LED matrix demo with specified brightness
function startMatrix(res, brightness) {
    const stopCommand = 'sudo pkill -f demo || true'; // Ignore errors from pkill
    exec(stopCommand, { cwd: demoPath }, (stopError, stopStdout, stopStderr) => {
        // Start new demo after ensuring any previous is stopped
        const startCommand = `sudo ./demo -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=64 --led-brightness=${brightness}`;
        exec(startCommand, { cwd: demoPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error starting demo: ${error}`);
                return res.status(500).send(`Error starting demo: ${error.message}`);
            }
            res.send(`Demo started successfully with brightness: ${brightness}`);
        });
    });
}

// Function to stop the LED matrix demo
function stopMatrix(res) {
    const stopCommand = 'sudo pkill -f demo';
    runDemoCommand(stopCommand, res);
}

// API endpoint to handle brightness adjustment and start the demo
app.get('/start-demo', (req, res) => {
    const brightness = req.query.brightness || 100; // Default brightness is 100%
    startMatrix(res, brightness);
});

// API endpoint to stop the demo
app.get('/stop-demo', (req, res) => {
    stopMatrix(res);
});



// ////////////////////////////////////////////TIME SHIT//////////////////////////////////////////////////////////////
app.get('/display-time', (req, res) => {
    const command = `sudo ./clock -f ../fonts/7x13.bdf --led-chain=1 --led-no-hardware-pulse --led-cols=64 --led-rows=64 -d "%A" -d "%H:%M:%S"`;
    exec(command, { cwd: datePath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing clock command: ${error}`);
            return res.status(500).send(`Error executing clock command: ${error.message}`);
        }
        res.send("Clock command executed successfully.");
    });
});

// Endpoint to stop the clock display
app.get('/stop-time', (req, res) => {
    exec('sudo pkill -f clock', { cwd: datePath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the clock: ${error}`);
            return res.status(500).send(`Error stopping the clock: ${error.message}`);
        }
        res.send("Clock display stopped successfully.");
    });
});
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
