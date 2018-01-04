/**
* @file BridgeEditControl.C
* @Author Daniel Le
* @date November 26, 2017
* @brief Page for editing a bridge's data
*
* Creates a page that allows the user to modify the bridge's name, IP address, port number, and/or location
* Displays an error if an invalid input is given or if in the case of port number, the port is already taken
*
*
*
**/
#include <stdio.h>
#include <iostream>
#include <vector>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/Http/Message>
#include <Wt/Http/Client>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Array>
#include <Wt/Json/Parser>
#include <Wt/WLogger>
#include <algorithm>

#include "BridgeEditControl.h"
#include "Session.h"
#include "Bridge.h"
#include "BridgeUserIds.h"

using namespace Wt;
using namespace std;

BridgeEditControlWidget::BridgeEditControlWidget(Session *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session)
{
	setContentAlignment(AlignCenter);
	setStyleClass("highscores");
}

/*
* 
* @brief Updates page
*
* Reloads the page to refresh the list of existing bridges and reset the form for entering 
* bridge name, IP address, port number, and location
* @return Void.
**/
void BridgeEditControlWidget::update()
{
	clear();
	//get URL info
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("ip=");								//get ip
	string subString = address.substr(pos + 3);
	size_t endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");								//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);

	thisBridge = new Bridge();
	thisBridge = session_->getBridge(ip,port);

	//display user info in top left corner
	/*string firstName = session_->firstName();
	string lastName = session_->lastName();
	WText *userInfo_ = new WText(this);
	userInfo_->setTextAlignment(AlignmentFlag::AlignLeft);
	userInfo_->setText("Hello, " + firstName + " " + lastName);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());*/

	this->addWidget(new WText("Bridge Name: "));
	nameEdit_ = new WLineEdit(this);
	nameEdit_->setText(thisBridge->getBridgeName());
	this->addWidget(new WBreak());

	this->addWidget(new WText("IP Address: "));
	ipEdit_ = new WLineEdit(this);
	ipEdit_->setText(thisBridge->getIpAddress());
	this->addWidget(new WBreak());

	this->addWidget(new WText("Port Number: "));
	portEdit_ = new WLineEdit(this);
	portEdit_->setText(to_string(thisBridge->getPortNumber()));
	this->addWidget(new WBreak());

	this->addWidget(new WText("Location: "));
	locationEdit_ = new WLineEdit(this);
	locationEdit_->setText(thisBridge->getLocation());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	WPushButton *button
		= new WPushButton("Update Bridge", this);
	WPushButton *returnButton
		= new WPushButton("Return Bridge", this);
	returnButton->setLink("/?_=/bridge");
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	errorText_ = new WText(this);

	button->clicked().connect(this, &BridgeEditControlWidget::updateEdit);
	(boost::bind(&BridgeEditControlWidget::handleHttpResponse, this));
	(boost::bind(&BridgeEditControlWidget::connect, this));
	(boost::bind(&BridgeEditControlWidget::updateEdit, this));
}

//Connect to client
Http::Client * BridgeEditControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}
//Handles POST call
void BridgeEditControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		if (response.body().find("error") != -1) {
			update();
			errorText_->setText("Please link the bridge");
		}
		else {
			//Parse the username from the response JSON
			Json::Array jsonArr;
			Json::parse(response.body(), jsonArr);
			Json::Object userObj = jsonArr.at(0);
			Json::Object tempObj = userObj.get("success");
			string username = tempObj.get("username").toString();

			//Get the bridge name, ip address, port number, and location
			string name = nameEdit_->text().toUTF8();
			string ipAddr = ipEdit_->text().toUTF8();
			string portNum = portEdit_->text().toUTF8();
			string location = locationEdit_->text().toUTF8();

			Bridge *oldBridge = session_->getBridge(ip, port);
			//Creates a new bridge and adds it to the database
			Bridge *temp = new Bridge();
			temp->setBridgeName(name);
			temp->setIpAddress(ipAddr);
			int x = stoi(portNum);
			temp->setPortNumber(x);
			temp->setUserId(username);
			temp->setLocation(location);
		
			session_->updateBridge(oldBridge,temp);


		}
	}

}
//Checks input and makes POST call
void BridgeEditControlWidget::updateEdit() {
	errorText_->setText("");
	string portNum = portEdit_->text().toUTF8();
	string ipNum = ipEdit_->text().toUTF8();
	//Check if port is a number
	if (!portNum.empty() && find_if(portNum.begin(), portNum.end(), [](char c) { return !std::isdigit(c); }) == portNum.end()) {
		vector<Bridge> bridges = session_->getBridges();
		vector<int> ports;
		bool foundPort = false;
		//Creates a vector of all the port numbers
		for (size_t i = 0; i<bridges.size(); i++) {
			ports.push_back(bridges[i].getPortNumber());
		}

		//Check if the port number is registered yet
		for (size_t i = 0; i<ports.size(); i++) {
			if (ports[i] == stoi(portNum)) {
				foundPort = true;
			}
		}

		if (foundPort == false || portNum.compare(port)==0) {
			Http::Client *client = BridgeEditControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"devicetype\" : \"danny\"}");
			client->done().connect(boost::bind(&BridgeEditControlWidget::handleHttpResponse, this, _1, _2));
			client->post("http://" + ipNum + ":" + portNum + "/api", *msg);
			errorText_->setText("Bridge Updated.");
		}
		else {
			errorText_->setText("That port number is already taken.");
		}
	}
	
	else {
		errorText_->setText("Please enter a number for the port.");
	}
}
