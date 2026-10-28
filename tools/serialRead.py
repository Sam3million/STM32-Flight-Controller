import serial, pandas as pd

ser = serial.Serial("/dev/tty.usbmodem2103", 115200, timeout=15)
for i in range(2):
    rows = []
    measureName = ""
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        
        print(line)
        
        if line == "END": break
        if line and line[0] == "i": measureName = line
        if line and line[0].isdigit(): rows.append(line.split(","))
    print(measureName + "\n")
    spl = measureName.split(",")
    measuredValue = spl[1]
    pd.DataFrame(rows, columns=spl).astype(int).to_csv(measuredValue + "-recorded.csv", index=False)