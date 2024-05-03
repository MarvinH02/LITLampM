const express = require('express');
const { exec } = require('child_process');
const cors = require('cors');
const sharp = require('sharp');
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

// Function to display image
function displayImage(imagePath, res) {
    sharp(imagePath)
        .resize(64, 64) // Assuming 64x64 LED matrix
        .toBuffer()
        .then(buffer => {
            const command = `sudo ./led-image-viewer --led-no-hardware-pulse --led-rows=64 --led-cols=64 --led-slowdown-gpio=2 -`;
            const child = exec(command, { cwd: demoPath });
            child.stdin.write(buffer);
            child.stdin.end();

            child.on('exit', () => res.send('Image displayed successfully.'));
        })
        .catch(err => {
            console.error(`Error processing image: ${err}`);
            res.status(500).send(`Error processing image: ${err.message}`);
        });
}

// API endpoint to display image
app.post('/display-image', (req, res) => {
    const { imagePath } = req.body;
    displayImage(imagePath, res);
});



// API endpoints
app.get('/stop-demo', (req, res) => {
    startMatrix(res);
});

app.get('/start-demo', (req, res) => {
    stopMatrix(res);
});




app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
