#!/usr/bin/python3

import RPi.GPIO as GPIO
from time import sleep
import os


# Pin config
GPIO.setmode(GPIO.BCM)
INPUT_PIN = 27

GPIO.setup(INPUT_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)

print("Monitoring input voltage... Press Ctrl-C to exit.")


sleep(1)
try:
    while True:
        if GPIO.input(INPUT_PIN) == GPIO.HIGH:
            print("Power loss detected! Shutting down...")
            os.system("sudo shutdown -h now")
            break

        sleep(1)

except KeyboardInterrupt:
    print("Exiting...")

finally:
    GPIO.cleanup()
