import serial
import pandas as pd

ser = serial.Serial("COM3", baudrate=115200, timeout=1)  # timeout = 1 second
test = []

try:
    print("COLLECTING....")
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                print(line)
                line_list = line.split(",")
                test.append(line_list)

except KeyboardInterrupt:
    df = pd.DataFrame(test, columns=["speed","error","total error","PWM","time","total_time"])
    # 1st: kp, 2nd: ki, 3rd: compare value
    # compare value is set to 30
    df.to_csv("official_four_feet_test/test_down_30_5_30_again.csv", index=False)
    print("Stopped by user")
finally:
    ser.close()