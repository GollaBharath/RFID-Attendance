import serial
import datetime

PORT = 'COM9'  
BAUD_RATE = 115200
current_datetime = datetime.datetime.now()
#print(current_datetime)
formatted_datetime = current_datetime.strftime("%H:%M:%S")
# print(formatted_datetime)
current_date = datetime.date.today()
# print(current_date)
try:
    with serial.Serial(PORT, BAUD_RATE, timeout=1) as ser, open("attendance_log.txt", "a") as log:
        print("Logging started... Press Ctrl+C to stop.")
        while True:
            line = ser.readline().decode().strip()
            if line.startswith("UID:"):
                line = line+f", Date : {current_date}, Time : {formatted_datetime}\n"
                print(line)
                log.write(f"{line}")
except KeyboardInterrupt:
    print("Logging stopped.")
except Exception as e:
    print(f"Error: {e}")
