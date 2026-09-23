# 04 — UART

Sends characters to a host terminal over the Nano's USB-serial bridge.
Counts 000–999, one number per line. 326 B

## How it works

**UART framing.** The line idles high. Each character is sent as a start bit
(0), 8 data bits least significant first, then a stop bit (1). There is no
clock wire, so both ends must agree the bit rate in advance — a mismatch
produces garbage characters rather than an error.

**Baud rate.** UBRR = F_CPU / (16 × baud) − 1 = 16,000,000 / (16 × 9600) − 1
= 103. That gives an actual rate of 9,615 baud, a 0.16% error, well within
the ~2% a receiver tolerates.

**Sending.** UCSR0B bit 3 (TXEN0) enables the transmitter. Before each byte,
UCSR0A bit 5 (UDRE0) is polled: the hardware sets it when the data register
is free. Writing to UDR0 hands the byte over, and the hardware clocks out the
whole frame.

**Digits.** UART sends bytes, not numbers, so values are converted to ASCII:
the digits '0'–'9' are codes 48–57, so '0' + digit gives the character.
A three-digit number is split with n / 100, (n / 10) % 10 and n % 10.
'\r' returns the cursor to the left margin, '\n' moves down a line.

## Reading it

    screen /dev/cu.usbserial-XXXXXXXX 9600

Exit with Ctrl+A, K, Y. Close screen before flashing — only one program can
hold the serial port.

## Limitations

- Transmit only; the receiver (RXEN0) is not enabled.
- Blocking: uart_putc spins until the hardware is ready, so at 9600 baud each
  character costs about 1 ms of CPU time.
- uart_print_number handles 0–999 only, and always prints three digits with
  leading zeros.
