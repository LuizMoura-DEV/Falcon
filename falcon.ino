#include <Ultrasonic.h>
#include <EEPROM.h>

#define trig    10
#define echo    8

//Sensor Linha
#define sensorFD  A2
#define sensorFE  A3
#define sensorTD  A0
#define sensorTE  A1
//Controle Motor
#define motorDF   9
#define motorDT   6
#define motorEF   3
#define motorET   5
//Botões e LEDs
#define btStart   7
#define btModo    4
#define ledR      11
#define ledG      12
#define ledB      13

Ultrasonic ultrasonic(trig, echo);

const int valorBranco = 970, timeVirar = 150;
bool linha = false, esquerda = false, direita = false, inimigoFrente = false;
unsigned long delayTimeEscape;

void setup() {
    //Ativando monitor serial    
    Serial.begin(9600);

    //Carregando Sensor Distancia
    
    //Definindo Entradas    
    pinMode(sensorFD,   INPUT);
    pinMode(sensorFE,   INPUT);
    pinMode(sensorTD,   INPUT);
    pinMode(sensorTE,   INPUT);
    pinMode(btStart,    INPUT);
    pinMode(btModo,     INPUT);    

    //Definindo Saídas
    pinMode(motorEF,    OUTPUT);
    pinMode(motorET,    OUTPUT);
    pinMode(motorDF,    OUTPUT);
    pinMode(motorDT,    OUTPUT);
    pinMode(ledR,       OUTPUT);
    pinMode(ledG,       OUTPUT);
    pinMode(ledB,       OUTPUT);

    motor(0,0,0,0);
}

/*void loop(){
    teste();
}*/

void loop() {
    digitalWrite(ledB,LOW);
    digitalWrite(ledG,LOW);
    digitalWrite(ledR,LOW);
    inimigo();
    if(inimigoFrente){
        motor(100,0,100,0);
        digitalWrite(ledB,HIGH);
    }else{
        motor(40,0,40,0);
        digitalWrite(ledB,LOW);
    }
    escape();
}

void teste(){
    //Exibir valores dos sensores
    
    inimigo();
    Serial.println("Valores Lidos");
    Serial.print("Iinimigo: ");
    Serial.print(inimigoFrente);
    Serial.print(" -|- Frente: Direita= ");
    Serial.print(analogRead(sensorFD));
    Serial.print(" / Esquerda= ");
    Serial.print(analogRead(sensorFE));
    Serial.print(" -|- Atraz: Direita= ");
    Serial.print(analogRead(sensorTD));
    Serial.print(" / Esquerda= ");
    Serial.println(analogRead(sensorTE));
}

void motor(int escF, int escT, int dirF, int dirT){
    //Recebe porcentagem para cada motor e converte para PWM
    analogWrite(motorEF,    map(escF, 0, 100, 0 ,255));
    analogWrite(motorET,    map(escT, 0, 100, 0 ,255));
    analogWrite(motorDF,    map(dirF, 0, 100, 0 ,255));
    analogWrite(motorDT,    map(dirT, 0, 100, 0 ,255));
}

float sonico(){
    float dist;
    long microsec= ultrasonic.timing();
    return ultrasonic.convert(microsec, Ultrasonic::CM); 
}

void inimigo() {
    float dist = sonico();
    bool inimigo = false;
    if(dist > 2 && dist < 15){
        inimigoFrente = true;
    }else if(dist >=15 && dist < 50){        
        inimigoFrente = false;
    }else{        
        inimigoFrente = false;
    }
}

bool IR(int sensor){
    //Verifica sensor IR recebendo a porta
    if(analogRead(sensor) < valorBranco){
        return true;
    }else{
        return false;
    }
}

void atraz(){
    motor(0,100,0,100);
}

void escape(){
    //Verifica se linha TRUE gira ate false 
    while(IR(sensorFE)){
        atraz();
        escapeT();
        esquerda = true;
        delayTimeEscape = millis();
    }
    while(IR(sensorFD)){
        atraz();
        escapeT();
        direita = true;
        delayTimeEscape = millis();
    }
    if(esquerda){
        if((millis() - delayTimeEscape) < timeVirar){
            motor(80,0,0,100);
        }else{
            esquerda =    false;
            direita =     false;
        }
    }
    if(direita){
        if((millis() - delayTimeEscape) < timeVirar){            
            motor(0,100,80,0);
        }else{
            esquerda =    false;
            direita =     false;
        }
    }
    escapeT();
}

void escapeT(){   
    while(IR(sensorTD)){
        motor(0,0,50,0);
        escape();
    }
    while(IR(sensorTE)){
        motor(50,0,0,0);
        escape();
    }  
}

