
//?sensores
#define vsense A0
#define wsense A1
#define tsense A2
#define hsense A3
#define magnet A4


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
 
int ecode = 0;

int s1 = 5;
bool s1s = 0; //s1 state

bool agua = 0;

int buzzer = 4;

int light = 2;

//servo pin = 6

void setup(){

    s.begin(115200);

    pinMode(wsense, INPUT);
    pinMode(magnet, INPUT);
    pinMode(fan, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(buzzer, OUTPUT);

    digitalWrite(l_error, LOW);
}

void humedad(){
    / ?Valores humedad
    for(int i = 0; i <= 100; i++){
        h1 = h1 + analogRead(Sensor1);
        delay(1);
        h1 = h1 / 100.00;            
   }        
}

void s1error(){
while(true){
    humedad();      
    digitalWrite(l_error, HIGH);
    btn1s = digitalRead(btn1);
    ecode = 1;
    Serial.println(h1);
    if(btn1s == 1){
        //? 0
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        
        delay(1500);

        //? 1
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);
        delay(300);
        digitalWrite(buzzer, HIGH);
        delay(1000);
        digitalWrite(buzzer, LOW);        
        Serial.println(ecode);
    }
    if(h1 < 1000){
      return; 
    }
    
}

void RA(){
  
  humedad();

    //?riego
    if(h1 < threshold1l){
        digitalWrite(s2, LOW);
        digitalWrite(s3, LOW);
        digitalWrite(s4, LOW);
        digitalWrite(s5, LOW);
        digitalWrite(s1, HIGH);
        while(true){
           agua = digitalRead(Ws);
           delay(10);

           if(agua == 0){   
              digitalWrite(s2, LOW);
              digitalWrite(s3, LOW);
              digitalWrite(s4, LOW);
              digitalWrite(s5, LOW);
              digitalWrite(s1, LOW);
              return;
           }                    
          humedad();
          if(h1 > threshold1h){
            digitalWrite(s1, LOW);
            s1s = 0;
            return;
          }
        }
    }
    if(h1 > 1000){
        s1error();
    }
    if(h1 >= threshold1h){
        digitalWrite(s1, LOW);
        s1s = 0;
    }
    Serial.println(h1);
}

void loop(){

    //voltaje
    voltaje = analogRead(vsense);
    voltaje = (voltaje * 5.0) / 1023.0;

    if(voltaje <= 11.5){
      ecode = 3;
    }
    else if(voltage >= 13){
      ecode = 4;
    }
    //?Temperatura y ventilador
    temp = analogRead(tsense);

    if(temp >= thresholdth){
        digitalWrite(fan, HIGH);
    }
    else if(temp > 40){
      ecode = 5;
    }
    else{
        digitalWrite(fan, LOW);
    }

    if(temp <= thresholdtl){
      digitalWrite(light, HIGH);
    }
    else{
      digitalWrite(light, LOW);
    }

    //? Sensor de agua
    agua = digitalRead(wsense);
    delay(10);

    if(agua == 0){   
        ecode = 2;
    }

    //?Comunicación con app


    if (agua == 1){
        RA();
    }
  Serial.println(ecode);

}
