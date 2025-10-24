#define PIN_LEFT_MOTOR_SPEED 5
#define PIN_LEFT_MOTOR_FORWARD A0            
#define PIN_LEFT_MOTOR_REVERSE A1
#define PIN_LEFT_ENCODER 2
   
#define PIN_RIGHT_MOTOR_SPEED 6
#define PIN_RIGHT_MOTOR_FORWARD A2            
#define PIN_RIGHT_MOTOR_REVERSE A3
#define PIN_RIGHT_ENCODER 3

#define SERIAL_BAUD_RATE 9600
int speed = 0;
int left_encoder_count=0;
int right_encoder_count=0;
int status = -1;

void left_encoder(){
  left_encoder_count++;  
}

void right_encoder(){
  right_encoder_count++;  
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  pinMode(PIN_LEFT_MOTOR_SPEED, OUTPUT);
  analogWrite(PIN_LEFT_MOTOR_SPEED, 0);
  pinMode(PIN_LEFT_MOTOR_FORWARD, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_REVERSE, OUTPUT);

  pinMode(PIN_RIGHT_MOTOR_SPEED, OUTPUT);
  analogWrite(PIN_RIGHT_MOTOR_SPEED, 0);
  pinMode(PIN_RIGHT_MOTOR_FORWARD, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_REVERSE, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_LEFT_ENCODER), left_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_RIGHT_ENCODER), right_encoder, RISING);

}

void stop(){
  digitalWrite(PIN_LEFT_MOTOR_FORWARD, LOW);
  digitalWrite(PIN_LEFT_MOTOR_REVERSE, LOW);
  analogWrite(PIN_LEFT_MOTOR_SPEED, 0);

  digitalWrite(PIN_RIGHT_MOTOR_FORWARD, LOW);
  digitalWrite(PIN_RIGHT_MOTOR_REVERSE, LOW);
  analogWrite(PIN_RIGHT_MOTOR_SPEED, 0);
  status = 0;
}
void leftMotorMovement(bool forward){
  digitalWrite(PIN_LEFT_MOTOR_FORWARD, forward ? HIGH : LOW);
  digitalWrite(PIN_LEFT_MOTOR_REVERSE, forward ? LOW : HIGH);
  analogWrite(PIN_LEFT_MOTOR_SPEED, speed);
}

void rightMotorMovement(bool forward){
  digitalWrite(PIN_RIGHT_MOTOR_FORWARD, forward ? HIGH : LOW);
  digitalWrite(PIN_RIGHT_MOTOR_REVERSE, forward ? LOW : HIGH);
  analogWrite(PIN_RIGHT_MOTOR_SPEED, speed);
}

void movement(){
  status = 1;
  // if (speed < 0){
  //   leftMotorMovement(false);
  //   rightMotorMovement(false);
  // }else{
  //   leftMotorMovement(true);
  //   rightMotorMovement(true);
  // }                                                                                                                                                                                     
}
void setSpeed(int value){
  speed = value; 
}
void rotate(int angle){
  status = 2;
}
void sendMessage(String mess){
  Serial.println(mess);
}

uint8_t XORChecksum8(const byte *data, size_t dataLength)
{
  uint8_t value = 0;
  for (size_t i = 0; i < dataLength; i++)
  {
    value ^= (uint8_t)data[i];
  }
  return ~value;
}

void getStatus(){
  String m = "ACK, getting status: ";
  if(status == -1){
    m += "Status: idle";
  }else if (status == 0){
    m += "Status: stopped";
  }else if (status == 1){
    m += "Status: moving";
  }else if (status == 2){
    m += "Status: rotating";
  }else{
    m+= "Status: error";
  }
  sendMessage(m);
}

void message(String com){ 
  if(com.length() == 0){
    return;
  }
  String normalizedMess = com.substring(0,1);
  if(normalizedMess == "M"){
    movement(); 
    sendMessage("ACK, moving");
    int instruction = com.substring(1, com.length()).toInt();
    // ruch o zadana odleglosc
  }else if (normalizedMess == "R"){
    int instruction = com.substring(1, com.length()).toInt();
    rotate(instruction);
    sendMessage("ACK, rotating");
    // obrot o zadana liczbe krokow
  }else if(normalizedMess == "V"){
    int instruction = com.substring(1, com.length()).toInt();
    setSpeed(instruction); // predkosc liniowa
    sendMessage("ACK, setting speed");
  }else if(normalizedMess == "S"){
    stop();
    sendMessage("ACK, stopping");
  }else if(normalizedMess == "B")
  {
    sendMessage("ACK, getting sonar info in cm");
  }else if(normalizedMess == "I"){
    sendMessage("ACK, getting IR info");

  }else if(normalizedMess == "E"){
    sendMessage("ACK, changing engines");
  }
  else if(normalizedMess == "X"){
    getStatus();
  }
  else{
    sendMessage("NACK, wrong message");
  }
}

void loop() {
  // speed = 0;
  while(Serial.available()<=0){
    // WAIT
  }
  String command = Serial.readStringUntil('\n');
  message(parser(command));
  left_encoder_count=0;
  right_encoder_count=0;
}

String parser(String message){
  int commaIndex = message.indexOf(',');
  if (commaIndex < 0){
    sendMessage("NACK, wrong message properties");
    return "";
  }
  String data = message.substring(0, commaIndex);     
  String checksum = message.substring(commaIndex + 1); 

  byte dane[data.length()];
  data.getBytes(dane, data.length() + 1);
  uint8_t calculated = XORChecksum8(dane, data.length());
  uint8_t recevied = strtoul(checksum.c_str(), NULL, 16);
  if (calculated == recevied){
    return data;
  }else{
    sendMessage("NACK, wrong control sum");
    return "";
  }
}