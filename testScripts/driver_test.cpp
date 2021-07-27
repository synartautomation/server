#include "i2c_drive.h"
uint8_t encoders[]= {0x0c,0x0a,0x0b};
I2C_drive motors(4,encoders,0x1d);
int pwm[4]={0,0,0,0};

int main(){
	int test;
	cin>>test;
	while(1){
		pwm[0]=test;
		pwm[1]=test;
		pwm[2]=test;
		pwm[3]=test;
		motors.drive(pwm,true);
	}
	return 0;
}
