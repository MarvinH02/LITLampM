const express = require('express');
const fs = require('fs');
const { exec } = require('child_process');
const cors = require('cors');
const app = express();
const port = 3000;

app.use(cors());
app.use(express.json());  

const demoPath = '/home/g10/rpi-rgb-led-matrix/examples-api-use';
const datePath = '/home/g10/rpi-rgb-led-matrix/examples-api-use';
const colorPath = '/home/g10/rpi-rgb-led-matrix/examples-api-use/color_display';
const commandFilePath = '/tmp/snake_commands';
const weatherPythonFilePath = '/home/g10/Desktop/weathertest';
const cityWeatherPath = '/home/g10/Desktop/weathertest';
const WeatherMeasurementPath = '/home/g10/Desktop/weathertest';
const CustomImagePath = '/home/g10/Desktop/rpi-rgb-led-matrix/examples-api-use';





///////////////////////////////////////////////////////ONOFF//////////////////////////////////////////////////////////////////////////////
// Function to run the demo command
function runDemoCommand(command, res) {
    exec(command, { cwd: demoPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing command '${command}': ${error}`);
            return res.status(500).send(`Error: ${error.message}`);
        }
        res.send(`Command executed: ${stdout}`);
    });
}

// Function to start the LED matrix demo with specified brightness
function startMatrix(res, brightness) {
    const stopCommand = 'sudo pkill -f demo || true';
    exec(stopCommand, { cwd: demoPath }, (stopError) => {
        const startCommand = `sudo ./demo -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=64 --led-brightness=${brightness}`;
        console.log(startCommand);
        exec(startCommand, { cwd: demoPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error starting demo: ${error}`);
                return res.status(500).send(`Error starting demo: ${error.message}`);
            }
            res.send(`Demo started with brightness: ${brightness}`);
        });
    });
}
function stopMatrix(res) {
    const stopCommand = 'sudo pkill -f demo';
    runDemoCommand(stopCommand, res);
}

function displayColor(res, color) {
    const stopCommand = `sudo pkill -f DisplayColor || true`; 
    exec(stopCommand, { cwd: colorPath }, (stopError, stopStdout, stopStderr) => {
        const startCommand = `sudo ./DisplayColor --color ${color}`;
        exec(startCommand, { cwd: colorPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error starting demo: ${error}`);
                return res.status(500).send(`Error starting Display-Color: ${error.message}`);
            }
            res.send(`Display color started: ${color}`);
        });
    });
}
app.get('/stop-color', (req, res) => {
    exec(`sudo pkill -f DisplayColor`, { cwd: colorPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the clock: ${error}`);
            return res.status(500).send(`Error stopping the clock: ${error.message}`);
        }
        res.send("Color display stopped.");
    });
});

// API endpoint to handle brightness and start the demo
app.get('/start-demo', (req, res) => {
    const brightness = req.query.brightness || 100; // Default brightness is 100%
    stopAllDisplays(() => {
        startMatrix(res, brightness);
    });
});

// API endpoint to stop the demo
app.get('/stop-demo', (req, res) => {
    stopMatrix(res);
});

app.get('/display-color', (req, res) => {
    const color = req.query.color || ffffff
    stopAllDisplays(() => {
        displayColor(res, color);
    });
});


// CUSTOM WEATHER DISPLAY


app.post('/run-python', (req, res) => {
    const { cityName } = req.body;  // Get city name from request body
    const command = `python3 stockholm1read.py "${cityName}"`;
    const stopCommand = `sudo pkill -f stockholm1read.py`;
    console.log(`Executing command: ${command}`);
    exec(stopCommand, { cwd: weatherPythonFilePath }, (stopError) => {
        exec(command, { cwd: weatherPythonFilePath }, (error) => {
            if (error) {
                console.error(`Error executing weather command: ${error}`);
                return res.status(500).send(`Error: ${error.message}`);
            }
            res.send("Weather command executed successfully.");
        });
    });
});


app.get('/display-weather', (req, res) => {
    const command = `sudo ./display_weather --led-chain=1 --led-pwm-lsb-nanoseconds=50 --led-pwm-dither-bits=2 --led-cols=64 --led-rows=64`;
    stopAllDisplays(() => {
        exec(command, { cwd: cityWeatherPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error executing room temperature command: ${error}`);
                return res.status(500).send(`Error executing clock command: ${error.message}`);
            }
            res.send("City temperature command executed.");
        });
    });
});

app.get('/stop-weather', (req, res) => {
    exec('sudo pkill -f display_weather', { cwd: cityWeatherPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the room temperature: ${error}`);
            return res.status(500).send(`Error stopping the clock: ${error.message}`);
        }
        res.send("City temperature display stopped.");
    });
});




app.get('/measure-room-temperature', (req, res) => {
    const command = './bme280_sensor.py';
    exec(command, { cwd: WeatherMeasurementPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error executing room temperature command: ${error}`);
            return res.status(500).send(`Error executing room temp reading command: ${error.message}`);
        }
        res.send("Room temperature command executed.");
    });
});



app.get('/display-room-temperature', (req, res) => {
    const command = `sudo ./temperature_display --led-chain=1 --led-pwm-lsb-nanoseconds=50 --led-pwm-dither-bits=2 --led-cols=64 --led-rows=64`;
    stopAllDisplays(() => {
        exec(command, { cwd: WeatherMeasurementPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error executing room temperature command: ${error}`);
                return res.status(500).send(`Error executing room-temp command: ${error.message}`);
            }
            res.send("Room temperature command executed.");
        });
    });
});

app.get('/stop-room-temperature', (req, res) => {
    exec('sudo pkill -f temperature_display', { cwd: WeatherMeasurementPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the room temperature: ${error}`);
            return res.status(500).send(`Error stopping the room-temp: ${error.message}`);
        }
        res.send("Room temperature display stopped.");
    });
});

// ////////////////////////////////////////////TIME & DATE//////////////////////////////////////////////////////////////
app.get('/display-time', (req, res) => {
    const command = `sudo ./clock -f ../fonts/6x13.bdf --led-chain=1 --led-pwm-lsb-nanoseconds=50 --led-pwm-dither-bits=2 --led-cols=64 --led-rows=64  -d "%A" -d "%Y-%m-%d" -d "%H:%M:%S"`;
    stopAllDisplays(() => {
        exec(command, { cwd: datePath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error executing clock command: ${error}`);
                return res.status(500).send(`Error executing clock command: ${error.message}`);
            }
            res.send("Clock command executed.");
        });
    });
});

app.get('/stop-time', (req, res) => {
    exec('sudo pkill -f clock', { cwd: datePath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the clock: ${error}`);
            return res.status(500).send(`Error stopping the clock: ${error.message}`);
        }
        res.send("Clock display stopped.");
    });
});

function displayImage(res, imageFileName) {
    const stopCommand = `sudo pkill -f image-example || true`; 
    const startCommand = `sudo ./image-example /home/g10/Desktop/Project-MQTTpanel-master/files/usr/local/share/rgbmatrix/images/${imageFileName} --led-cols=64 --led-rows=64`;
    console.log(startCommand);
    exec(stopCommand, { cwd: demoPath }, (stopError, stopStdout, stopStderr) => {
        exec(startCommand, { cwd: demoPath }, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error starting demo: ${error}`);
                return res.status(500).send(`Error starting demo: ${error.message}`);
            }
            res.send('Demo started: ' + stdout);
        }); 
    });

}
app.get('/stop-stock-image', (req, res) => {
    exec(`sudo pkill -f image-example || true`, { cwd: demoPath }, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error stopping the image showing: ${error}`);
            return res.status(500).send(`Error stopping the image showing: ${error.message}`);
        }
        res.send("Stock image display stopped.");
    });
});


app.get('/image-stock', (req, res) => {
    const imageFileName = req.query.image;
    console.log(imageFileName);
    stopAllDisplays(() => {
        displayImage(res, imageFileName);
    });
});



////////////////////////////////////////////////SNAKE GAME///////////////////////////////////////////////////////////////////
app.get('/start-snake', (req, res) => {
    stopAllDisplays(() => {
        exec(`make && sudo ./snake`, { cwd: '/home/g10/Desktop/games/snake' }, (error) => {
            if (error) {
                console.error(`Error starting snake game: ${error}`);
                return res.status(500).send(`Error: ${error.message}`);
            }
            res.send("Snake game started");
        });
    });
});

app.get('/stop-snake', (req, res) => {
    exec(`sudo pkill -f snake`, (error) => {
        if (error) {
            console.error(`Error stopping snake game: ${error}`);
            return res.status(500).send(`Error: ${error.message}`);
        }
        res.send("Snake game stopped");
    });
});

app.post('/control-snake', (req, res) => {
    const { command } = req.body;
    if (!['w', 'a', 's', 'd'].includes(command)) {
        return res.status(400).send("Invalid command");
    }
    console.log(`Received command: ${command}`);  
    fs.writeFile(commandFilePath, command, { flag: 'w' }, (error) => {
        if (error) {
            console.error(`Error writing command to file: ${error}`);
            return res.status(500).send(`Error: ${error.message}`);
        }
        console.log(`Command '${command}' written to file.`);
        res.send(`Command '${command}' sent to snake game.`);
    });
});

app.get('/start-timer', (req, res) => {
    const time = req.query.time;
    console.log(`Timer started for ${time} seconds`);
    setTimeout(() => {
        console.log("Timer ended. Stopping all displays.");
        stopAllDisplays(() => {
            console.log("All displays stopped after timer.");
            res.send("Timer ended and all displays stopped successfully.");
        });
    }, time * 1000);
});
function stopAllDisplays(callback) {
    const stopCommands = [
        { command: 'sudo pkill -f demo', cwd: '/home/g10/rpi-rgb-led-matrix/examples-api-use' },
        { command: 'sudo pkill -f DisplayColor', cwd: '/home/g10/rpi-rgb-led-matrix/examples-api-use/color_display' },
        { command: 'sudo pkill -f display_weather', cwd: '/home/g10/Desktop/weathertest' },
        { command: 'sudo pkill -f temperature_display', cwd: '/home/g10/Desktop/weathertest' },
        { command: 'sudo pkill -f clock', cwd: '/home/g10/rpi-rgb-led-matrix/examples-api-use' },
        { command: 'sudo pkill -f image-example', cwd: '/home/g10/rpi-rgb-led-matrix/examples-api-use' },
        { command: 'sudo pkill -f snake', cwd: '/home/g10/Desktop/games/snake' }
    ];
    let commandsStopped = 0;
    stopCommands.forEach((item) => {
        exec(item.command, { cwd: item.cwd }, (error) => {
            if (error) {
                console.error(`Error stopping process with command '${item.command}' in '${item.cwd}': ${error}`);
            }
            commandsStopped++;
            if (commandsStopped == stopCommands.length)
                callback();
        });
    });

}

// API endpoint to trigger stopping all displays
app.get('/stop-all-displays', (req, res) => {
    stopAllDisplays(() => {
        res.send("Stopping all display functions."); 
    });
});

//Display if running
app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});



