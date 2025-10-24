import serial
import time
import datetime
# pip install pyserial
command_history = []
def print_commands():
    print("======")
    print("M(cm) - moving cm into given direction (positive forward, negative backwards)")
    print("R(deg) - rotating by given angle (positive forward, negative backwards)")
    print("V(v) - setting constant speed")
    print("B - getting sonar info in cm")
    print("I - IR readings")
    print("X - robot status")
    print("E - engine set up")
    print("history - history")
    print("help - help")
    print("save - save log")
    print("q - quit")
    print("======")

def save_logs():
    global command_history
    if len(command_history) == 0:
        return
    with open('arduino_logs.txt', 'a+') as f:
        for line in command_history:
            f.write(line + '\n')
    command_history = []
    print("logs saved")

def history():
    print("===History===")
    for line in command_history:
        print(line)
    print("=============")

def save_mess(mes, author):
    s = str(datetime.datetime.now()) +" " + author + ": " + mes
    command_history.append(s)

PORT = "/dev/cu.usbmodem101"
BAUDRATE = 9600

MAX_RETRIES = 3
BASE_DELAY = 0.5
TIMEOUT = 3.0

def check_sum_calc(data):
    data = [ord(d) for d in data]
    value = 0
    for x in data:
        value ^= x
    return (~value)& 0xFF

def message_with_numbers_validation(prompt):
    prompt = prompt.replace(" ", "")
    try:
        value = int(prompt[1:])
    except ValueError:
        return -1
    return value


arduino = serial.Serial(PORT, BAUDRATE, timeout=1)
# arduino = serial.serial_for_url('rfc2217://localhost:5544', baudrate=BAUDRATE, timeout=2)
time.sleep(5)


print("Arduino connected")
print("Start sending commands to arduino (help for command list, q to quit)")

def send_to_arduino(command):
    attempt = 0

    while attempt < MAX_RETRIES:
        start_time = time.time()
        got_response = False

        while time.time() - start_time < TIMEOUT:
            mess = f"{command},{check_sum_calc(command):02X}"
            arduino.write((mess + "\n").encode())
            save_mess(command, "user")
            response = arduino.readline().decode().strip()
            if response:
                print("Arduino:", response)
                save_mess(response, "arduino")
                got_response = True
                break
        if got_response:
            break
        attempt += 1
        delay = BASE_DELAY * (2 ** attempt)
        time.sleep(delay)

    else:
        print("Request timeout")

try:
    while True:
        cmd = input()
        if len(cmd) == 0:
            print("Empty command (help for command list)")
            continue
        match cmd:
            case "help":
                print_commands()
            case "save":
                save_logs()
            case "history":
                history()
            case "q":
                print("Program stopping")
                break
            case _:
                if cmd and cmd[0] in ('M', 'R', 'V'):
                    result = message_with_numbers_validation(cmd)
                    if result == -1:
                        print(f"Wrong structure, try again hint: {cmd[0]}<number>")
                    else:
                        send_to_arduino(cmd)
                elif cmd and cmd[0] in ('S', 'B', 'X', 'I', 'E'):
                    send_to_arduino(cmd)
                else:
                    send_to_arduino(cmd)

except KeyboardInterrupt:
    print("Program stopped.")

finally:
    arduino.close()
