#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <CurieBLE.h>

//? display
LiquidCrystal_I2C lcd(0x3F,20,4);


//?sensores
#define vsense A0
#define wsense A1
#define tsense A2
#define hsense A3
int magnet = 7;


//?variables
float h1 = 0;

float temp = 0;

float threshold1l = 100; //min threshold
float threshold1h = 80;//max threshold

float thresholdtl = 22.5; //min threshold
float thresholdth = 24.5;//max threshold

float toolow_humidity = 90;
float toomuch_humidity = 190;
float toolow_temp = 22;
float toomuch_temp = 25;


int fan = 3;
int fans = 0; //fan state
 
int ecode = 0;

int s1 = 5;
bool s1s = 0; //s1 state

bool agua = 0;

int buzzer = 4;

int light = 2;
int lights = 0; //light state

float voltaje = 0;
int val = 0;
//servo pin = 6
int roof = 0;

Servo servo;

//bluetooth
  //services 
BLEService invService("ff27e5d2-4685-4acf-bb0d-9b8e78ef3fe2");


  //characteristics
BLECharCharacteristic tempCharacteristic("413d567e-b751-4db7-bda5-a156784413c5", BLERead);
BLECharCharacteristic pumpCharacteristic("363e31ae-1e66-4573-aa91-011ae4f416c3", BLERead);
BLECharCharacteristic lightCharacteristic("1a9fdd82-734b-4332-8429-b2dfbe375f4b", BLERead);
BLECharCharacteristic humidityCharacteristic("fb76b4b0-61e3-4b03-ad8a-76d7cd82d50d", BLERead);
BLECharCharacteristic roofCharacteristic("61f4de7b-4736-47fc-bf6f-f6f23f618a7b", BLEWrite);
BLECharCharacteristic ventCharacteristic("065b22c2-0bf7-4344-9fbc-d93bb9d53437", BLERead);
BLECharCharacteristic magnetCharacteristic("25934e48-2063-43d9-8329-47f8ac73f352", BLERead);
BLECharCharacteristic errorCharacteristic("c7c92757-504d-4aa8-9b4a-94c923592aaf", BLERead);


BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)

void setup(){

    Serial.begin(9600);

    servo.attach(6);

    pinMode(wsense, INPUT);
    pinMode(magnet, INPUT);
    pinMode(fan, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(light, OUTPUT);
    pinMode(13, OUTPUT);

    lcd.init();
    lcd.backlight();

    //BLE
    Serial.print("pre init");
    

    blePeripheral.setLocalName("inv");
    BLE.setAdvertisedServiceUuid(invService.uuid());

    //Attributes
    blePeripheral.addAttribute(invService);
    blePeripheral.addAttribute(tempCharacteristic);
    blePeripheral.addAttribute(pumpCharacteristic);
    blePeripheral.addAttribute(lightCharacteristic);
    blePeripheral.addAttribute(humidityCharacteristic);
    blePeripheral.addAttribute(roofCharacteristic);
    blePeripheral.addAttribute(ventCharacteristic);
    blePeripheral.addAttribute(magnetCharacteristic);
    blePeripheral.addAttribute(errorCharacteristic);
/*
    invService.addCharacteristic(tempCharacteristic);
    invService.addCharacteristic(pumpCharacteristic);
    invService.addCharacteristic(lightCharacteristic);
    invService.addCharacteristic(humidityCharacteristic);
    invService.addCharacteristic(roofCharacteristic);
    invService.addCharacteristic(magnetCharacteristic);
    invService.addCharacteristic(errorCharacteristic);*/ 
    
    blePeripheral.begin();
    Serial.print("ble init");   
}

void ble(){
  Serial.print("B");
  BLECentral central = blePeripheral.central();
  if(central){
    
    humedad();
    
    roof = digitalRead(magnet);
    
    
 
    tempCharacteristic.setValue(temp);

    pumpCharacteristic.setValue(s1s);
    
    lightCharacteristic.setValue(lights);
    
    humidityCharacteristic.setValue(h1);
    
    ventCharacteristic.setValue(fans);
    
    magnetCharacteristic.setValue(6);
    
    errorCharacteristic.setValue(ecode);

    if(roofCharacteristic.written()&& roofCharacteristic.value()){
      Serial.println("1");
      magnetCharacteristic.setValue(7);
      if(digitalRead(magnet) == 0){
        Close();
        digitalWrite(13, LOW);
      }
      if(digitalRead(magnet) == 1){
        Open();
        digitalWrite(13, HIGH);
      }
    }
  }
  else{
    return;
  }
}

void Open(){
  roof = digitalRead(magnet);
  if(roof == 1){
    servo.write(90);
  }
}

void Close(){
  roof = digitalRead(magnet);
  if(roof == 0){
    servo.write(0);
  }
}

void humedad(){
    /*/ ?Valores humedad
    for(int i = 0; i <= 100; i++){
        h1 = h1 + analogRead(hsense);
        delay(1);
        h1 = h1 / 100.00;           
   } */
   h1 = analogRead(hsense);
   h1 = map(h1, 0, 1023, 0, 100);       
  // Serial.println("h"); 
}

void bz(){
  for(int i = 0; i < 5; i++){
    digitalWrite(buzzer, HIGH);
    delay(250);
    digitalWrite(buzzer, LOW);
    delay(250);
  }
}
void error(int ecode){
  
  lcd.clear();
  
  switch(ecode){
    case 1:
      lcd.print("Error ");
      //lcd.setCursor(0,7);
      lcd.print(ecode);
      bz();
      humedad();
      if(h1 < 1000 && ecode == 1){
        ecode = 0;
      }
      break;
      
    case 2:
      lcd.print("Error ");
      //lcd.setCursor(0,7);
      lcd.print(ecode);
      
      bz();
      
      if(digitalRead(wsense) == 1 && ecode == 2){
        ecode = 0;
      }
      break;
      
    case 3:
      lcd.print("Error ");
      //lcd.setCursor(0,7);
      lcd.print(ecode);  
        
      bz();
      
      voltaje = (analogRead(vsense) * 5.0) / 1023.0;
      if(voltaje > 11.5 && ecode == 3){
        ecode = 0;
      }
      break;
      
    case 4:
      lcd.print("Error ");
      //lcd.setCursor(0,7);
      lcd.print(ecode);  
        
      bz();
      
      voltaje = (analogRead(vsense) * 5.0) / 1023.0;
      if(voltaje < 13 && ecode == 4){
        ecode = 0;
      }
      break;
      
    case 5:
      lcd.print("Error ");
      //lcd.setCursor(0,7);
      lcd.print(ecode);
          
      bz();
      
      if((5.0 * analogRead(tsense) * 100.0) / 1023.0 < 40 && ecode == 5){
        ecode = 0;
      }
      break;
      
  }
}

void Display(){
  //states
  lcd.setCursor(0,0);
  if(s1s == 1){
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print("Pump on");
    delay(1500);
  }
  
  
  if(lights == 1){
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print("Light on");    
    delay(1500);
  }


  if(fans == 1){
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print("Fan on");
    delay(1500);     
  }


  if(roof == 0){
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print("Roof open"); 
    delay(1500);    
  }
    if(roof == 1){
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,0);
    lcd.print("Roof closed"); 
    delay(1500);    
  }

  //values
  lcd.setCursor(0,1);

  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print(" C");

  lcd.setCursor(0,2);

  lcd.print("Humedad: ");
  lcd.print(h1);
  lcd.print(" %");
  lcd.print(" ");
  Serial.println("Display");
}

void RA(){
  
  humedad();

    //?riego
    if(h1 <= threshold1l){
        digitalWrite(s1, HIGH);
        s1s = 1;
        
           agua = digitalRead(wsense);
           delay(10);

           if(agua == 0){   
              ecode = 2;
              error(ecode);
              digitalWrite(s1, LOW);
              return;
           }                    
          humedad();
          Display();
     //     ble();
          if(h1 > threshold1h){
            digitalWrite(s1, LOW);
            s1s = 0;
          }
        }
    
    if(h1 > 1000){
        ecode = 1;
        error(ecode);
    }
    //Serial.println(h1);
}

void loop(){

    //voltaje
    voltaje = (analogRead(vsense) * 1023.0) / 5.0;

    if(voltaje <= 11.5){
      ecode = 3;
      error(ecode);
    }
   /* else if(voltaje >= 13){
      ecode = 4;
      error(ecode);
    }*/
    //?Temperatura y ventilador
    temp = (analogRead(tsense) * 1023.0) / 5.0;
    temp = temp * 10.0;
    temp = temp / 10.0;
    temp = temp / 1000.0;
    //temp = temp / 1000.0;
    
    if(temp >= thresholdth){
        digitalWrite(fan, HIGH);
        fans = 1;
    }
    else if(temp > 40){
      ecode = 5;
      error(ecode);
    }
    else{
        digitalWrite(fan, LOW);
        fans = 0;
    }

    if(temp <= thresholdtl){
      digitalWrite(light, HIGH);
      lights = 1;
    }
    else{
      digitalWrite(light, LOW);
      lights = 0;
    }

    //? Sensor de agua
    agua = bool(digitalRead(wsense));
    delay(10);
  //Serial.println(ecode);
  
  if(ecode != 0){
    error(ecode);
  }
  Display();
  BLECentral central = blePeripheral.central();
  if(!central){
    Serial.println("no");
    BLECentral central = blePeripheral.central();
    if(central){
      Serial.println(central.address());
    }
  }
  
  
    if(agua == 0){   
        ecode = 2;
        error(ecode);
    }

    //?Comunicación con app

    if (agua == 1){
        RA();
    }


    ble();
}
