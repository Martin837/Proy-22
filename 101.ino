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
float threshold1h = 180;//max threshold

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

//servo pin = 6
int roof = 0;

Servo servo;

//bluetooth
  //services 
BLEService tempService("ff27e5d2-4685-4acf-bb0d-9b8e78ef3fe2");
BLEService pumpService("030f42d5-576e-47d6-8eeb-722e2ade1e76");
BLEService lightService("4f97110d-c8f4-4488-96b8-923d043e9d7a");
BLEService humidityService("297406a6-b976-41de-af77-7acb1bae7857");
BLEService roofService("297406a6-b976-41de-af77-7acb1bae7857");
BLEService ventService("297406a6-b976-41de-af77-7acb1bae7857");
BLEService magnetService("da060191-1f87-4ad2-88c1-0dca8008bd03");
BLEService errorService("ffcedbaf-2a7f-47c1-85ef-8138891396cc");

  //characteristics
BLECharCharacteristic tempCharacteristic("413d567e-b751-4db7-bda5-a156784413c5", BLEWrite);
BLECharCharacteristic pumpCharacteristic("363e31ae-1e66-4573-aa91-011ae4f416c3", BLEWrite);
BLECharCharacteristic lightCharacteristic("1a9fdd82-734b-4332-8429-b2dfbe375f4b", BLEWrite);
BLECharCharacteristic humidityCharacteristic("fb76b4b0-61e3-4b03-ad8a-76d7cd82d50d", BLEWrite);
BLECharCharacteristic roofCharacteristic("61f4de7b-4736-47fc-bf6f-f6f23f618a7b", BLERead);
BLECharCharacteristic ventCharacteristic("065b22c2-0bf7-4344-9fbc-d93bb9d53437", BLEWrite);
BLECharCharacteristic magnetCharacteristic("25934e48-2063-43d9-8329-47f8ac73f352", BLEWrite);
BLECharCharacteristic errorCharacteristic("c7c92757-504d-4aa8-9b4a-94c923592aaf", BLEWrite);


BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)

void setup(){

    Serial.begin(9600);

    servo.attach(6);

    pinMode(wsense, INPUT);
    pinMode(magnet, INPUT);
    pinMode(fan, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(buzzer, OUTPUT);

    lcd.init();
    lcd.backlight();

    //BLE
    blePeripheral.setLocalName("invernadero");

    //Attributes
    blePeripheral.addAttribute(tempService);
    blePeripheral.addAttribute(pumpService);
    blePeripheral.addAttribute(lightService);
    blePeripheral.addAttribute(humidityService);
    blePeripheral.addAttribute(roofService);
    blePeripheral.addAttribute(ventService);
    blePeripheral.addAttribute(magnetService);
    blePeripheral.addAttribute(errorService);
    blePeripheral.addAttribute(tempCharacteristic);
    blePeripheral.addAttribute(pumpCharacteristic);
    blePeripheral.addAttribute(lightCharacteristic);
    blePeripheral.addAttribute(humidityCharacteristic);
    blePeripheral.addAttribute(roofCharacteristic);
    blePeripheral.addAttribute(ventCharacteristic);
    blePeripheral.addAttribute(magnetCharacteristic);
    blePeripheral.addAttribute(errorCharacteristic);

    blePeripheral.begin();
}

void ble(){
  BLECentral central = blePeripheral.central();
  if(central){
    
    humedad();
    
    roof = digitalRead(magnet);
    
    blePeripheral.poll();
    
    blePeripheral.setAdvertisedServiceUuid(tempService.uuid());
    tempCharacteristic.setValue(temp);
    
    blePeripheral.setAdvertisedServiceUuid(pumpService.uuid());
    pumpCharacteristic.setValue(s1s);
    
    blePeripheral.setAdvertisedServiceUuid(lightService.uuid());
    lightCharacteristic.setValue(lights);
    
    blePeripheral.setAdvertisedServiceUuid(humidityService.uuid());
    humidityCharacteristic.setValue(h1);
    
    blePeripheral.setAdvertisedServiceUuid(ventService.uuid());
    ventCharacteristic.setValue(fans);
    
    blePeripheral.setAdvertisedServiceUuid(magnetService.uuid());
    magnetCharacteristic.setValue(roof);
    
    blePeripheral.setAdvertisedServiceUuid(errorService.uuid());
    errorCharacteristic.setValue(ecode);

    if(roofCharacteristic.written()){
      if(digitalRead(magnet) == 0){
        Close();
      }
      if(digitalRead(magnet) == 1){
        Open();
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
    // ?Valores humedad
    for(int i = 0; i <= 100; i++){
        h1 = h1 + analogRead(hsense);
        delay(1);
        h1 = h1 / 100.00;           
   }        
   Serial.println("h"); 
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

  //values
  lcd.setCursor(0,1);

  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("C");

  lcd.setCursor(0,2);

  lcd.print("Humedad: ");
  lcd.print(h1);
  lcd.print(" %");
  Serial.println("Display");
}

void RA(){
  
  humedad();

    //?riego
    if(h1 < threshold1l){
        digitalWrite(s1, HIGH);
        s1s = 1;
        while(true){
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
          ble();
          if(h1 > threshold1h){
            digitalWrite(s1, LOW);
            s1s = 0;
            return;
          }
        }
    }
    if(h1 > 1000){
        ecode = 1;
        error(ecode);
    }
    if(h1 >= threshold1h){
        digitalWrite(s1, LOW);
        s1s = 0;
    }
    Serial.println(h1);
}

void loop(){

    //voltaje
    voltaje = (analogRead(vsense) * 1023.0) / 5.0;

    if(voltaje <= 11.5){
      ecode = 3;
      error(ecode);
    }
    else if(voltaje >= 13){
      ecode = 4;
      error(ecode);
    }
    //?Temperatura y ventilador
    temp = (analogRead(tsense) / 1023.0) * 5.0;
    temp = temp * 100.0;

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
  Serial.println(ecode);
  
  if(ecode != 0){
    error(ecode);
  }
  Display();
  Serial.println(voltaje);
  
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
