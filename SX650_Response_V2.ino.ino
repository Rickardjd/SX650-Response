// Version 2
// Recoded for Arduino MKRZero
// Date 5th Oct. 2018
// Changes
// 8_10_18  Added RON levels and ACK commands
//          Changed RS485 control to TX RX both on.





const int STX=0x02;   //STX Character
const int ETX=0x03;   //ETX Character  
const int ACK=0x06;   //ACK Character
byte RON=7;        //RON State. RON 0 = No response, RON 7 = Respond
const byte LED_Serial=LED_BUILTIN;  //Serial recieve LED
const byte debugPin=1;   //Debug switch input
boolean debug=0;      //Debug switch. Controls serial writeback. Add switch to control.


void setup() {
 //   pinMode(debugPin, INPUT_PULLUP);    //Sets debugPin as input with internal pullup
 //   debug=!digitalRead(debugPin);
 //   if (debug){
      Serial.begin(9600);  //Setup hardware serial port    
 //   }
  
  Serial1.begin(19200);  //Setup hardware serial port  
  pinMode(LED_Serial, OUTPUT);   //Serial recieve LED
  pinMode(A5, OUTPUT);   // !RE  MKR485 Board uses A5 to RE
  pinMode(A6, OUTPUT);   // DE MKR485 Board uses A6 to DE
  if (debug){
    Serial.println("Starting serial");
  }
  TXMode(true);  

  
}

String command;

void loop() {
  debug=!digitalRead(debugPin); 
  digitalWrite(LED_Serial,LOW);
  TXMode(false);   //Set rs485 into RX mode
  if (Serial1.available()){

    char c = Serial1.read();
    if (c == STX) {   //clear command string when STX is recieved
      command = "";
    }
    else if (c == ETX){    //Parse the command string once ETX is recieved.
    TXMode(true);
      //delay(20);      
      digitalWrite(LED_Serial, HIGH);
      parseCommand(command); 
      command = "";    
    }
    else {
      command += c;
     // digitalWrite(LED_Serial, HIGH);
    //  delay(10);
     // digitalWrite(LED_Serial,LOW);
  }
  }
}

void parseCommand(String com){
  String part1;   //Address 
  String part2;   //Format
  String part3;   //Command
  String Response;  //Response string
  if (debug){
    Serial.print("Input :");
    Serial.println(com);
  }

  //Break the String Com into its 3 parts
  part1 = com.substring(0, com.indexOf(";"));
  part2 = com.substring((com.indexOf(";")+1), com.indexOf(":"));
  part3 = com.substring((com.indexOf(":"))+1);
  if (debug){
    Serial.print("Part1:");
    Serial.println(part1);
    Serial.print("Part2:");
    Serial.println(part2);
    Serial.print("Part3:");
    Serial.println(part3);

  }
  
  if (RON==0){
    Serial1.write(ACK); 
  }  

  
  //Process RON Commands
  if (part2.equals("RON")){
    if (part3.equals("0")){  
      if (debug){
        Serial.println("RON 0");
        }
      //TXMode(true);  
      Serial1.write(ACK);
      //TXMode(false);
      RON=0;
      Response = part1 +";" + "RON";
      SendString(Response);
      }
    else if(part3.equals("7")){  
       if (debug){
          Serial.println("RON 7");
          }
       RON=7;
      }
    }
  
  //Respond to RON 7 command
  if (part2.equals("RON") and part3.equals("7")){  
    Response = part1 +";" + "RON";
    SendString(Response);
  }

  //Respond to 2020118 command
  if (part3.equals("2020118")){  
    Response = part1 +";" + part2 + ":" + "2020118F";
    SendString(Response);
  }

  //Respond to 2020150 command
  if (part3.equals("2020150")){  
    Response = part1 +";" + part2 + ":" + "2021500";
    SendString(Response);
  }
}

//Set rs485 into TX mode
void TXMode(boolean State){
//  if (State){
//    digitalWrite(A5,HIGH);   //Set rs485 into TX mode
//    digitalWrite(A6,HIGH);   //Set rs485 into TX mode
//  }
//  else {
//    digitalWrite(A5,LOW);   //Set rs485 into RX mode
//    digitalWrite(A6,LOW);   //Set rs485 into RX mode
//  }
  digitalWrite(A5, LOW);     //TX enabled
  digitalWrite(A6, HIGH);      //RX enabled
}


void SendString(String Command){
  TXMode(true);
  delay(20);
  Serial1.write(STX);
  Serial1.print(Command);
  Serial1.write(ETX);
 // delay(20);
 // TXMode(false);
  if (debug){
    Serial.print("Output :");
    Serial.println(Command);
  }

}




