# BASIC ROV CONTROL (UP,DOWN,TURN LEFT,TURN RIGHT,FORWARD,BACKWARDS)

#WITHOUT CLAW CONTROL



import pygame
import socket
import time


# arduino ethernet
arduino_ip = "192.168.1.50"
port = 8080
DEAD_ZONE = 0.1

# init pygame
pygame.init()
if pygame.joystick.get_count() == 0:
    print("No joystick found")
    exit()
joystick = pygame.joystick.Joystick(1)
joystick.init()
print("Joystick started")


def apply_dead_zone(value, threshold=DEAD_ZONE):
    return 0 if abs(value) < threshold else value

def get_motor_commands():
    pygame.event.pump()
    
    forward_back = -apply_dead_zone(joystick.get_axis(1))  
    baseSpeed = round(forward_back * 255)
    
    turn = apply_dead_zone(joystick.get_axis(2))  
    turnSpeed = round(turn * 255)

    leftMotor = max(min(baseSpeed + turnSpeed, 255), -255)
    rightMotor = max(min(baseSpeed - turnSpeed, 255), -255)

    up_thrust = joystick.get_axis(5)  
    down_thrust = joystick.get_axis(4)  
    upMotor = round(((up_thrust + 1) / 2) * 255)
    downMotor = round(((down_thrust + 1) / 2) * 255)

    return leftMotor, rightMotor, upMotor, downMotor

def connect_to_arduino():
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((arduino_ip, port))
        print(f"Connected to Arduino at {arduino_ip}:{port}")
        return sock
    except Exception as e:
        print("Connection error:", e)
        return None



client_socket = None

while True:
    if client_socket is None:
        print("Attempting to connect to Arduino...")
        client_socket = connect_to_arduino()
        if client_socket is None:
            print("Retrying connection in 3 seconds...")
            time.sleep(3)
            continue
    
    left, right, up, down = get_motor_commands()

    command = f"L:{left},R:{right},U:{up},D:{down}\n"

    try:
        client_socket.sendall(command.encode())
        print("Command sent:", command.strip())
    except Exception as e:
        print("Error during send:", e)
        client_socket.close()
        client_socket = None
        time.sleep(2)
        continue

    time.sleep(0.1) 
