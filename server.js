const express = require('express');
const { exec } = require('child_process');
const cors = require('cors');
const app = express();
const port = 3000;

app.use(cors());

const demoPath = '/home/g10/rpi-rgb-led-matrix/examples-api-use'; 


// Function to start the LED matrix demo
function startMatrix(res) {
    const startCommand = 'sudo ./demo -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=64';
    exec(startCommand, { cwd: demoPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error starting demo: ${error}`);
            return res.status(500).send(`Error starting demo: ${error.message}`);
        }
        res.send('Demo started successfully: ' + stdout);
    });
}

// Function to stop the LED matrix demo
function stopMatrix(res) {
    const stopCommand = 'sudo pkill -f demo';
    exec(stopCommand, { cwd: demoPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping demo: ${error}`);
            return res.status(500).send(`Error stopping demo: ${error.message}`);
        }
        res.send('Demo stopped successfully: ' + stdout);
    });
}

// API endpoints
app.get('/start-demo', (req, res) => {
    startMatrix(res);
});

app.get('/stop-demo', (req, res) => {
    stopMatrix(res);
});

app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
