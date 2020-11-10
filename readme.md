# M5Stick-C serial display

## Build instructions

Change the baudrate in the `#define BAUDRATE 9600` directive, build & upload with Arduino IDE

## Serial port setup on the PC

### Windows

To set up serial port on Windows for redirection, use

```
mode COM4: BAUD=115200 PARITY=N DATA=8 STOP=1
```

(use `mode` to find out the serial port the M5Stick is connected to)

Then redirect a command output to the serial port with:

```
tasklist > com4:
```

### Linux

I had to enable the usbserial module with `sudo modprobe usbserial`

Then redirect some output to `/dev/ttyUSB0`
