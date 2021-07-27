#include "i2c_drive.h"
#include <fstream>
#define MAXARMSPEED 15.0
#define MAXBOTSPEED 15.0
#define PI 3.142
#define JET_RADIUS 1
#define WHEELSIZE 0.25*PI


uint8_t encoders[]= {0x0c,0x0a,0x0b};
I2C_drive motors(4,encoders,0x1d);
int pwm[4]={0,0,0,0};
int getCurrentArmPos(){
	motors.read_float();

	//cout<<motors.value[0].val<<endl;
	return motors.value[0].val;
}
int armSpeed2PWM(int armSpeed){
    //MAXARMSPEED-->255
    //armSpeed   --> x
    //check typecasting
    return (armSpeed/MAXARMSPEED)*255;
}
int botSpeed2PWM(int botSpeed){
    //MAXBOTSPEED --> 255
    //botSpeed    --> x
    //check typecasting
    return (botSpeed/MAXBOTSPEED)*255;
}
int main()
{
	//~ while(1)
	//~ {
	//~ getCurrentArmPos();
	//~ }
int currPos=0,prevPos=0,storedPos,goalPos,flag;
string _storedPos="0";
ofstream encoderfile;
ifstream file("currentPosition.txt");
//~ encoderfile.open("currentPosition.txt",ios::in);
getline(file,_storedPos);
storedPos=stof(_storedPos);
cout<<"[STORED POSITION]"<<storedPos<<endl;
file.close();
motors.read_float();
if(motors.value[0].result[5]==0){
	flag=0;
}
else{
	cout<<"[FIRST TIME]"<<endl;
	flag=1;
}
currPos=storedPos;
cout<<"[CURRENT POSITION]"<<currPos<<endl;
cout<<"[PREV POS]"<<prevPos<<endl;
//~ cin>>goalPos;
encoderfile.open("currentPosition.txt");
//~ while(currPos<goalPos &&prevPos<goalPos){
	//~ motors.read_float();
	//~ currPos=motors.value[0].val+storedPos*flag;
	//~ motors.read_float();
	//~ pwm[0]=-255;
	//~ pwm[1]=0;
	//~ pwm[2]=0;
	//~ pwm[3]=0;
	//~ motors.drive(pwm,true);
	//~ prevPos=currPos;
	//~ }
	int count=0,botPos=0,stepCount,botSpeed,botStepDistance,armSpeed,armSweep;
	cout<<"SWEEP, SPEED, STEPCOUNT, STEPDISTANCE, BOTSPEED"<<endl;
	cin>>armSweep>>armSpeed>>stepCount>>botStepDistance>>botSpeed;
	while(count<stepCount){
        while(currPos<armSweep){
            //arm_motor.drive(armSpeed2PWM(armSpeed),FORWARD);
            motors.read_float();
            currPos=motors.value[0].val+storedPos*flag;
            pwm[0]=armSpeed2PWM(armSpeed);
            pwm[1]=0;
            pwm[2]=0;
            pwm[3]=0;
            motors.drive(pwm,true);
            prevPos=currPos;
        }
        while(currPos>-1*armSweep){
            //arm_motor.drive(armSpeed2PWM(armSpeed),REVERSE);
            motors.read_float();
            currPos=motors.value[0].val+storedPos*flag;
            pwm[0]=-1*armSpeed2PWM(armSpeed);
            pwm[1]=0;
            pwm[2]=0;
            pwm[3]=0;
            motors.drive(pwm,true);
            prevPos=currPos;
        }
        while(currPos<0){
            //arm_motor.drive(armSpeed2PWM(armSpeed),REVERSE);
            motors.read_float();
            currPos=motors.value[0].val+storedPos*flag;
            pwm[0]=armSpeed2PWM(armSpeed);
            pwm[1]=0;
            pwm[2]=0;
            pwm[3]=0;
            motors.drive(pwm,true);
            prevPos=currPos;
        }
        while(botPos*WHEELSIZE<botStepDistance*count+1){
            // left_motor.drive(botSpeed2PWM(botSpeed),FORWARD)
            // left_motor.drive(botSpeed2PWM(botSpeed),FORWARD)
            motors.read_float();
            botPos=motors.value[1].val+motors.value[2].val;
            pwm[0]=0;
            pwm[1]=botSpeed2PWM(botSpeed);
            pwm[2]=botSpeed2PWM(botSpeed);
            motors.drive(pwm,true);
        }
        count++;
}
	pwm[0]=0;
	pwm[1]=0;
	pwm[2]=0;
	pwm[3]=0;
	motors.drive(pwm,true);
	encoderfile<<currPos;
	encoderfile.close();
return 0;
}
