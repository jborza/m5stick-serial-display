# M5Stick-C serial display

To set up serial port on Windows for redirection, use

```
mode COM4: BAUD=115200 PARITY=N DATA=8 STOP=1
```

(use `mode` to find out the serial port the M5Stick is connected to)

Then redirect a command output to the serial port with:

```
tasklist > com4:
```