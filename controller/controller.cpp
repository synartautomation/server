#include <iostream>
#include <cstring>
#include <map>

#include "controller.hpp"
#include "../parser/parser.hpp"
#include "../misc/globals.hpp"
#include "../misc/map_get.hpp"
using namespace std;

Parser prs;
map<string,Controller::handler> handlers;
void manualControl(int a,int b,int c){
	cout<<"CONVERTED VALUES \n";
	cout<<a<<" "<<b<<" "<<c<<endl;
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
		cout<<prs.values[1]<<endl;
		cout<<prs.values[2]<<endl;
		cout<<prs.values[3]<<endl;
		cout<<prs.values[4]<<endl;
		cout<<prs.values[5]<<endl;
		cout<<prs.values[6]<<endl;
		// automaticMode(stoi(prs.values[1]),stoi(prs.values[2]),stoi(prs.values[3]),stoi(prs.values[4]),stoi(prs.values[5]));

		//respond with complete message
	}
	else if(key=="D"){
		char response[3]={1,1,1};
		cout<<"Diagnostics"<<endl;
		//array[3 values]= rundiagnostics()
		send(fd,response,strlen(response),0);
		//respond with 3 values
	}
	else if(key=="J"){
		cout<<"Joy input"<<endl;
		cout<<prs.values[1]<<endl;
		cout<<prs.values[2]<<endl;
		cout<<prs.values[3]<<endl;
		manualControl(stoi(prs.values[1]),stoi(prs.values[2]),stoi(prs.values[3]));

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
