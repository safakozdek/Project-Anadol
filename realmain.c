#define ticksForTurn 18
#define lumenInInt 400
#define kP 3
#define kD 0.1

char currentMode[15] = "TEST";
char currentState[15] = "IDLE"
bool executionFinished = false;

//Set these from another class with interrupt handler
char[] nextCommand = "" 
bool newCommandAvailable = false;

//For 90 degree turns
uint8_t tickCount = 0;

//For PD Controller
float lastError = 0;

void controlLoop(){
    while(!executionFinished){
        
        //Process new command, get next state & mode
        if(newCommandAvailable){
            printString(nextCommand);

            if(strcmp(nextCommand, "TEST") == 0){
                printString("TESTING\r\n");
                strcpy(currentMode, "TEST");
                strcpy(currentMode, "IDLE");

            } else if (strcmp(nextCommand, "AUTO") == 0){
                printString("AUTONOMOUS\r\n")
                strcpy(currentMode, "AUTO");
                strcpy(currentMode, "IDLE");

            } else if (strcmp(nextCommand, "LEFT") == 0){
                strcpy(currentState, "LEFT");
                tickCount = 0;
                leftLED();

            } else if (strcmp(nextCommand, "RIGHT") == 0){
                strcpy(currentState, "RIGHT");
                tickCount = 0;
                rightLED();

            } else if (strcmp(nextCommand, "FORWARD") == 0){
                strcpy(currentState, "FORWARD");
                forwardLED();

            } else if (strcmp(nextCommand, "BACK") == 0){
                strcpy(currentState, "BACK");
                backLED();

            } else if (strcmp(nextCommand, "STOP") == 0){
                strcpy(currentState, "IDLE");

            } else if (strcmp(nextCommand, "START") == 0){
                strcpy(currentState, "START");

            } else if (strcmp(nextCommand, "STATUS") == 0){
                printStatusInfo();
            }

            newCommandAvailable = false;
        }

        //Get to desired state
        if(strcmp(currentMode, "TEST") == 0){
            uint16_t leftLDRValue = getLeftLDR();
            uint16_t rightLDRValue = getRightLDR();
            float potentVal = getPotentiometer();
            
            //Stop if light detected
            if(leftLDRValue < lumenInInt | rightLDRValue < lumenInInt){
                setLeftMotor(0);
                setRightMotor(0);

            } else if {
                if(strcmp(currentState, "LEFT") == 0){
                    if(tickCount < ticksForTurn){
                        setLeftMotor(20) * potentVal;
                        setRightMotor(-20) * potentVal;
                    } else {
                        strcpy(currentState, "IDLE");
                    }

                } else if(strcmp(currentState, "RIGHT") == 0){
                    if(tickCount < ticksForTurn){
                        setLeftMotor(-20) * potentVal;
                        setRightMotor(20) * potentVal;
                    } else {
                        strcpy(currentState, "IDLE");
                    }

                } else if(strcmp(currentState, "FORWARD") == 0){
                    setLeftMotor(20) * potentVal;
                    setRightMotor(20) * potentVal;

                } else if(strcmp(currentState, "BACK") == 0){
                    setLeftMotor(-20) * potentVal;
                    setRightMotor(-20) * potentVal;

                } else if(strcmp(currentState, "IDLE") == 0){
                    turnOffLED();
                    setLeftMotor(0);
                    setRightMotor(0);
                }
            }
            
            
        } else if(strcmp(currentMode, "AUTO") == 0){
            
            if(strcmp(currentState, "START") == 0){
                moveAutonomous(30);
            } else if(strcmp(currentState, "IDLE") == 0){
                turnOffLED();
                setLeftMotor(0);
                setRightMotor(0);
            }

        }

    }
}

void moveAutonomous(float baseSpeed){
    uint16_t leftLDRValue = getLeftLDR();
    uint16_t rightLDRValue = getRightLDR();
    float averageLight = (leftLDRValue + rightLDRValue) / 2;

    float potentVal = getPotentiometer();
    float backSonar = getBackUltrasonic();
    float frontSonar = getFrontUltrasonic();

    float leftPower = baseSpeed;
    float rightPower = baseSpeed;

    float error = frontSonar - backSonar;
    float correction = (error * kP) + ((error - lastError) * kD);

    if(backSonar < 20 | frontSonar < 20 && abs(frontSonar-backSonar) < 1.5){
        leftPower = baseSpeed - 5;
        rightPower = baseSpeed + 5;
    } else if (backSonar > 30 | frontSonar > 30 && abs(frontSonar-backSonar) < 1.5){
        leftPower = baseSpeed + 5;
        rightPower = baseSpeed - 5;
    } else {
        leftPower = baseSpeed + correction;
        rightPower = baseSpeed - correction;
    }

    //Closing to the light
    if(averageLight < 600){
        leftPower *= (averageLight - 400) / 200;
        rightPower *= (averageLight - 400) / 200;
    } else if(averageLight < 400){
        printString("FINISH");
        strcpy(currentState, "IDLE");
        return;
    }

    setLeftMotor(leftPower);
    setRightMotor(rightPower);

    lastError = error;
}

void forwardLED(){
    //Turn on front LEDs, others off.
}

void backLED(){
    //Turn on back LEDs, others off.
}

void leftLED(){
    //LEDs at left blink twice a second, others off.
}

void rightLED(){
    //LEDs at right blink twice a second, others off.
}

void turnOffLED(){
    //Turn all LEDs off.
}

//Between -100 and 100
void setLeftMotor(float power){
    return 0;
}

//Between 0-100
void setRightMotor(float power){
    return 0;
}

//In centimeters
float getFrontUltrasonic(){
    return 0;
}

//In centimeters
float getBackUltrasonic(){
    return 0;
}

//Between 0-1023
uint16_t getLeftLDR(){
    return 0;
}

//Between 0-1023
uint16_t getRightLDR(){
    return 0;
}

//Between 0-1
float getPotentiometer(){
    //Get value
    float value = 0;

    //Remove noise
    if(value > 0.9){
        value = 1;
    } else if(value < 0.1){
        value = 0
    }
    return  value;
}

//Send value to computer
void printString(char[] value){
    
}

//Send car status info to computer
void printStatusInfo(){
    
}

void init(){
    
}

int main(){
    init();
    printString("TESTING\r\n");
    controlLoop();
}