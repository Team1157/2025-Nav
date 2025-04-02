// BASIC ROV CONTROL (UP,DOWN,TURN LEFT,TURN RIGHT,FORWARD,BACKWARDS)

//WITHOUT CLAW CONTROL

#include <SPI.h>
#include <Ethernet.h>


// ethernet config
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 50);
EthernetServer server(8080);

// motor pins
#define PWM1 5   // Left motor speed control
#define DIR1 A0  // Left motor direction control
#define PWM2 6   // Right motor speed control
#define DIR2 A1  // Right motor direction control
#define PWM3 9   // Vertical up motor speed control
#define DIR3 A2  // Vertical up motor direction control
#define PWM4 8  // Vertical down motor speed control
#define DIR4 A3  // Vertical down motor direction control


void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }  

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  
  Serial.println("Starting Ethernet...");
  Ethernet.begin(mac, ip);
  server.begin();
  delay(1000);
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

}

  void loop() {
    EthernetClient client = server.available();
    if (client) {
      Serial.println("Client connected!");
      String command = "";

      unsigned long lastTime = millis();
      
     
      while (client.connected()) {
   
        if (client.available()) {
          lastTime = millis();  
          char c = client.read();
          command += c;
          if (c == '\n') {  
            if (command.length() > 0) {
              Serial.print("Received: ");
              Serial.println(command);
              int leftMotor, rightMotor, upMotor, downMotor, clawPos;
              int parsed = sscanf(command.c_str(), "L:%d,R:%d,U:%d,D:%d",
                                  &leftMotor, &rightMotor, &upMotor, &downMotor);
              if (parsed == 4) {
                // process motor commands
                setMotor(PWM1, DIR1, leftMotor);
                setMotor(PWM2, DIR2, rightMotor);
                setVerticalMotors(upMotor, downMotor);
                
                Serial.println("Motor executed.");
              } else {
                Serial.println("Failed to parse command!");
                turnOffMotors();
              }
            }
            command = "";  // clear command after processing
          }
        }
        
        
        if (millis() - lastTime > 10000) {
          Serial.println("No activity detected. Waiting for new data...");
        
        }
      }
      
 
      Serial.println("Client disconnected.");
      turnOffMotors();
      delay(100);
    }
  }




// motor control
void setMotor(int pwmPin, int dirPin, int speed) {
  speed = constrain(speed, -255, 255);
  digitalWrite(dirPin, speed >= 0 ? HIGH : LOW);
  analogWrite(pwmPin, abs(speed));
}

void setVerticalMotors(int upSpeed, int downSpeed) {
  upSpeed = constrain(upSpeed, 0, 255);
  downSpeed = constrain(downSpeed, 0, 255);
  if (upSpeed > 0 && downSpeed == 0) {
    digitalWrite(DIR3, HIGH);
    digitalWrite(DIR4, HIGH);
    analogWrite(PWM3, upSpeed);
    analogWrite(PWM4, upSpeed);
  } else if (downSpeed > 0 && upSpeed == 0) {
    digitalWrite(DIR3, LOW);
    digitalWrite(DIR4, LOW);
    analogWrite(PWM3, downSpeed);
    analogWrite(PWM4, downSpeed);
  } else {
    analogWrite(PWM3, 0);
    analogWrite(PWM4, 0);
  }
}

void turnOffMotors() {
  setMotor(PWM1, DIR1, 0);
  setMotor(PWM2, DIR2, 0);
  setVerticalMotors(0, 0);
}
