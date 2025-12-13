import serial
import pandas as pd
import time

ser = serial.Serial("COM3", baudrate=115200, timeout=1)  # timeout = 1 second
test = []

try:
    print("COLLECTING....")
    start_time = time.time()
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                curr_time = time.time()
                str_time = "," + str(curr_time)
                line = line + str_time
                print(line)
                line_list = line.split(",")
                test.append(line_list)

except KeyboardInterrupt:
    df = pd.DataFrame(test, columns=["error", "time"])
    df.to_csv("test.csv", index=False)
    print("Stopped by user")
finally:
    ser.close()