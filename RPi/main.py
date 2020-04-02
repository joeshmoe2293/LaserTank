#!/usr/bin/env python
import serial
import sys

if __name__ == '__main__':
    if sys.version_info.major == 2:
        input = raw_input

    port = '/dev/ttyACM0'
    baud = 9600
    ser = serial.Serial(port, baud)

    if ser.is_open:
        line = ''
        count = 0
        max_tries = 3

        line = ser.readline().rstrip('\r\n')
        if line != '' and line != 'Ready! Waiting for enable...':
            import pdb; pdb.set_trace()

        while line != 'Enabled!' and count < max_tries:
            ser.write('GO')
            line = ser.readline().rstrip('\r\n')
            max_tries += 1

        if count < max_tries:
            str_inp = ''

            while str_inp != 'q':
                str_inp = input('Enter "{power left} {power right}" (or q to quit)')

                if str_inp != 'q':
                    try: 
                        left, right = str_inp.split(' ')
                        left = int(left)
                        right = int(right)
                        ser.write(str_inp + '\n')

                        line = ser.readline()
                        print('Received: {}'.format(line))
                    except ValueError:
                        print('Please re-input commands!')

        ser.close()
    else:
        print('Unable to open serial port for {}'.format(port))
