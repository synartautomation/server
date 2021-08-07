#include <iostream>
#include <cstring>
#include <map>
#include <ios>
#include <iomanip>
#include <sstream>
#include "../i2c_drive.h"
#include "controller.hpp"
#include "../parser/parser.hpp"
#include "../misc/globals.hpp"
#include "../misc/map_get.hpp"

#define PI 3.142
#define JET_RADIUS 1
#define WHEELSIZE 0.25*PI

using namespace std;
string feedbackString;
uint8_t encoders[]= {0x0c,0x0a,0x0b};
I2C_drive motors(4,encoders,0x1d);
int pwm[4]={0,0,0,0};

string padZeros(int input, int width)
{
    std::ostringstream out;
    out << std::internal << std::setfill('0') << std::setw(width) << input;
    return out.str();
}
void createFeedbackString(int connectionStatus,int arm_motor_angle,float left_motor_distance,float right_motor_distance){
	feedbackString= to_string(connectionStatus)+" "+padZeros(arm_motor_angle,3)+" "+padZeros(left_motor_distance,3)+" "+padZeros(right_motor_distance,3);
}


Parser prs;
map<string,Controller::handler> handlers;
void manualControl(int a,int b,int c){
	motors.read_float();
	pwm[0]=a;
	pwm[1]=b;
	pwm[2]=c;
	pwm[3]=0;
	motors.drive(pwm,true);
	createFeedbackString(1,motors.value[0].val,motors.value[0].val*WHEELSIZE,motors.value[1].val*WHEELSIZE);
	response=&feedbackString[0];
	send(fd,response,32,0);
}
void Controller::handleServerConnection(uint16_t fd)
{
	cout << "Controller: got connect from " << fd << endl;
}

void Controller::handleServerDisconnect(uint16_t fd)
{
	cout << "Controller: got disconnect from " << fd << endl;
}

void Controller::handleServerInput(uint16_t fd, char *buffer)
{
	cout << "Controller: got input '" << buffer << "'from " << fd << endl;
	prs.parse(buffer);
	for(string i : prs.values)
	{
		cout << "Controller::handleServerInput -> for: input: '" << i << "'.\n";
	}


	//get the first argument name
	string key;
	try
	{
		key = prs.values.at(0);
	}
	catch(out_of_range oor)
	{
		cerr << "no action" << endl;
	}

	//look it up in the handlers map
	// Controller::handler h = NULL;
	// if(map_get<string,Controller::handler>(&handlers, key, &h))
	// {
	// 	printf("Controller: handler address = '%x'\n",h);
	// 	//call the callback function
	// 	h(&prs.values);
	// }
	if(key=="A"){
		cout<<"Auto mode"<<endl;
		motors.read_float();
		pwm[0]=prs.values[1];
		pwm[1]=prs.values[2];
		pwm[2]=prs.values[3];
		pwm[3]=prs.values[4];
		motors.drive(pwm,true);
		createFeedbackString(1,motors.value[0].val,motors.value[0].val*WHEELSIZE,motors.value[1].val*WHEELSIZE);
		response=&feedbackString[0];
		send(fd,response,32,0);
	}
	else if(key=="0"){
		cout<<"DO NOTHING"<<endl;
	}
	else if(key=="P"){
		createFeedbackString(1,0,0,0);
		char *response;
		response=&feedbackString[0];
		send(fd,response,32,0);
	}
	else if(key=="F"){
		// motors.read_float();
		//createFeedbackString(1,arm,left,right);
		char *response;
		response=&feedbackString[0];
		send(fd,response,32,0);
	}
	else if(key=="D"){
		char response[3]={1,1,1};
		cout<<"Diagnostics"<<endl;
		//array[3 values]= rundiagnostics()
		send(fd,response,sizeof(response),0);
		//respond with 3 values
	}
	else if(key=="J"){
		cout<<"Joy input"<<endl;
		cout<<prs.values[1]<<endl;
		cout<<prs.values[2]<<endl;
		cout<<prs.values[3]<<endl;
		manualControl(stoi(prs.values[1]),stoi(prs.values[2]),stoi(prs.values[3]));
	}
	else if(key=="C"){
		cout<<"Calibration"<<endl;
		//Reset Pin 13
		// digitalWrite(13,0);
		// delay(500);
		// digitalWrite(13,1);
		// Write 0 to currentPosition.txt
	
	}
	else
	{
		cerr << "[ERROR] Controller: No such action (" << key << ")\n";
	}

	prs.values.clear();
}

void Controller::addHandler(string key, Controller::handler cb)
{
	handlers.insert(pair<string,Controller::handler>(key,cb));
}

void Controller::removeHandler(string key)
{
	cout << "Controller::removeHandler: key=" << key << endl;
}
