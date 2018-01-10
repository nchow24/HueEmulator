/** @file SingleGroupsControl.C
*  @brief Application for deleting and altering states of individual groups
*  @author Nicole Chow
*  @date Nov 28, 2017
*/

#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WSound>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>
#include <Wt/WSlider>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Http/Client>
#include <Wt/WFileUpload>
#include <Wt/WLogger>
#include <Wt/Utils>
#include "SingleGroupsControl.h"
#include "Session.h"

using namespace Wt;
using namespace std;

SingleGroupsControlWidget::SingleGroupsControlWidget(Session *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session)
{
	setContentAlignment(AlignCenter);
}

void SingleGroupsControlWidget::update()
{
	clear();

	//get user info from URL
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("user=");								//get userID
	string subString = address.substr(pos + 5);
	size_t endPos = subString.find("&");
	userID = subString.substr(0, endPos);
	pos = address.find("ip=");										//get ip
	subString = address.substr(pos + 3);
	endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");									//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);
	pos = address.find("groupid=");									//get groupID
	subString = address.substr(pos + 8);
	endPos = subString.find("&");
	groupID = subString.substr(0, endPos);

	deleteConfirm = false;

	//display user info in top left corner
	string firstName = session_->firstName();
	string lastName = session_->lastName();
	WText *userInfo_ = new WText(this);
	userInfo_->setTextAlignment(AlignmentFlag::AlignLeft);
	userInfo_->setText("Hello, " + firstName + " " + lastName);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());


	//get group info to display 
	Http::Client *client = SingleGroupsControlWidget::connect();
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID)) {
		WApplication::instance()->deferRendering();
	}
	groupInfoEdit_ = new WText(this);								//group name
	this->addWidget(new WBreak());
	groupLightsEdit_ = new WText(this);								//lights in the group
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change group name
	this->addWidget(new WText("Set New Group Name: "));
	nameEdit_ = new WLineEdit(this);												
	nameEdit_->setFocus();
	WPushButton *nameButton
		= new WPushButton("Change", this);										
	nameButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//preset light modes
	this->addWidget(new WText("Pre set light modes: "));
	WPushButton *sunsetModeButton
		= new WPushButton("Sunset Yellow", this);                     
	sunsetModeButton->setMargin(5, Left);
	WPushButton *oceanModeButton
		= new WPushButton("Ocean Blue", this);
	oceanModeButton->setMargin(5, Left);
	WPushButton *forestModeButton
		= new WPushButton("Forest Green", this);
	forestModeButton->setMargin(5, Left);
	WPushButton *bloodModeButton
		= new WPushButton("Blood Red", this);
	bloodModeButton->setMargin(5, Left);
	WPushButton *mustangModeButton
		= new WPushButton("Mustang Purple", this);
	mustangModeButton->setMargin(5, Left);
	WPushButton *fireModeButton
		= new WPushButton("Fire Orange", this);
	fireModeButton->setMargin(5, Left);
	WPushButton *fiftyModeButton
		= new WPushButton("50 Shades", this);
	fiftyModeButton->setMargin(5, Left);
	WPushButton *partyModeButton
		= new WPushButton("Party Mode w. Music (10s duration)", this);                    
	partyModeButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//turn on
	this->addWidget(new WText("Light on/off: "));
	WPushButton *onButton
		= new WPushButton("ON", this);                     
	onButton->setMargin(5, Left);

	//turn off
	WPushButton *offButton
		= new WPushButton("OFF", this);                     
	offButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change hue
	this->addWidget(new WText("Hue: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("0  "));
	hueScaleSlider_ = new WSlider(this);					
	hueScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	hueScaleSlider_->setMinimum(0);
	hueScaleSlider_->setMaximum(65535);
	hueScaleSlider_->setValue(100);
	hueScaleSlider_->setTickInterval(10000);
	hueScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	hueScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  65535"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change brightness
	this->addWidget(new WText("Brightness: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("1  "));
	briScaleSlider_ = new WSlider(this);					
	briScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	briScaleSlider_->setMinimum(1);
	briScaleSlider_->setMaximum(254);
	briScaleSlider_->setValue(100);
	briScaleSlider_->setTickInterval(50);
	briScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	briScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  254"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change saturation
	this->addWidget(new WText("Saturation: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("0  "));
	satScaleSlider_ = new WSlider(this);				
	satScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	satScaleSlider_->setMinimum(0);
	satScaleSlider_->setMaximum(254);
	satScaleSlider_->setValue(100);
	satScaleSlider_->setTickInterval(50);
	satScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	satScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  254"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change transition time
	this->addWidget(new WText("Transition Time: (multiple of 100ms) "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("1  (100ms)"));
	transitionScaleSlider_ = new WSlider(this);					
	transitionScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	transitionScaleSlider_->setMinimum(1);
	transitionScaleSlider_->setMaximum(20);
	transitionScaleSlider_->setValue(4);
	transitionScaleSlider_->setTickInterval(2);
	transitionScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	transitionScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  20 (2 seconds)"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//add lights
	this->addWidget(new WText("Add Light: "));
	addChoices_ = new WComboBox(this);											
	WPushButton *addButton
		= new WPushButton("Add", this);										//submit button
	addButton->setMargin(5, Left);
	addChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//remove lights
	this->addWidget(new WText("Remove Light: "));
	removeChoices_ = new WComboBox(this);												
	WPushButton *removeButton
		= new WPushButton("Remove", this);										
	removeButton->setMargin(5, Left);
	removeChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//copy group
	this->addWidget(new WText("Make a copy of  this group (same name and lights):"));
	WPushButton *copyButton
		= new WPushButton("Copy", this);
	copyButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	
	//delete group
	WPushButton *deleteButton							
		= new WPushButton("Delete This Group", this);

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	change_ = new WText(this);                          //status of a light change
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	upload = new Wt::WFileUpload(this);
	upload->setFileTextSize(40000);
	this->addWidget(new WBreak());
	Wt::WPushButton *uploadButton = new Wt::WPushButton("Send", this);

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//return to groups page
	WPushButton *groupButton
		= new WPushButton("Return to My Groups", this);
	groupButton->setLink("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	groupButton->setMargin(10, Left);
	
	//Goes to Schedule page
	WPushButton *scheduleButton
		= new WPushButton("Make Scheduler", this);
	scheduleButton->setLink("/?_=/groupscheduler?user=" + userID + "%26ip=" + ip + "%26port=" + port + "%26groupid=" + groupID);
	scheduleButton->setMargin(10, Left);
	
	//return to lights page
	WPushButton *lightButton
		= new WPushButton("Return to My Lights", this);
	lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	lightButton->setMargin(10, Left);
	WPushButton *returnButton							
		= new WPushButton("Return To Bridge", this);


	// Upload when the button is clicked.
	uploadButton->clicked().connect(upload, &Wt::WFileUpload::upload);
	uploadButton->clicked().connect(uploadButton, &Wt::WPushButton::disable);
	// Upload automatically when the user entered a file.
	//upload->changed().connect(upload, &WFileUpload::upload);
	//upload->changed().connect(uploadButton, &Wt::WPushButton::disable);
	// React to a succesfull upload.
	upload->uploaded().connect(this, &SingleGroupsControlWidget::fileUploaded);
	// React to a fileupload problem.
	upload->fileTooLarge().connect(this, &SingleGroupsControlWidget::fileTooLarge);

	onButton->clicked().connect(this, &SingleGroupsControlWidget::on);
	copyButton->clicked().connect(this, &SingleGroupsControlWidget::copy);
	partyModeButton->clicked().connect(this, &SingleGroupsControlWidget::partyMode);
	mustangModeButton->clicked().connect(this, &SingleGroupsControlWidget::mustangMode);
	oceanModeButton->clicked().connect(this, &SingleGroupsControlWidget::oceanMode);
	bloodModeButton->clicked().connect(this, &SingleGroupsControlWidget::bloodMode);
	fireModeButton->clicked().connect(this, &SingleGroupsControlWidget::fireMode);
	fiftyModeButton->clicked().connect(this, &SingleGroupsControlWidget::fiftyMode);
	forestModeButton->clicked().connect(this, &SingleGroupsControlWidget::forestMode);
	sunsetModeButton->clicked().connect(this, &SingleGroupsControlWidget::sunsetMode);
	addButton->clicked().connect(this, &SingleGroupsControlWidget::addLights);
	removeButton->clicked().connect(this, &SingleGroupsControlWidget::removeLights);
	nameButton->clicked().connect(this, &SingleGroupsControlWidget::name);
	offButton->clicked().connect(this, &SingleGroupsControlWidget::off);
	returnButton->clicked().connect(this, &SingleGroupsControlWidget::returnBridge);
	deleteButton->clicked().connect(this, &SingleGroupsControlWidget::deleteGroup);
	briScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::bright);
	satScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::sat);
	hueScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::hue);
	transitionScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::transition);

	(boost::bind(&SingleGroupsControlWidget::copy, this));
	(boost::bind(&SingleGroupsControlWidget::hue, this));
	(boost::bind(&SingleGroupsControlWidget::name, this));
	(boost::bind(&SingleGroupsControlWidget::bright, this));
	(boost::bind(&SingleGroupsControlWidget::sat, this));
	(boost::bind(&SingleGroupsControlWidget::on, this));
	(boost::bind(&SingleGroupsControlWidget::off, this));
	(boost::bind(&SingleGroupsControlWidget::returnBridge, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponse, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&SingleGroupsControlWidget::connect, this));
	(boost::bind(&SingleGroupsControlWidget::deleteGroup, this));
	(boost::bind(&SingleGroupsControlWidget::addLights, this));
	(boost::bind(&SingleGroupsControlWidget::removeLights, this));
	(boost::bind(&SingleGroupsControlWidget::transition, this));
	(boost::bind(&SingleGroupsControlWidget::partyMode, this));
	(boost::bind(&SingleGroupsControlWidget::sunsetMode, this));
	(boost::bind(&SingleGroupsControlWidget::bloodMode, this));
	(boost::bind(&SingleGroupsControlWidget::oceanMode, this));
	(boost::bind(&SingleGroupsControlWidget::forestMode, this));
	(boost::bind(&SingleGroupsControlWidget::fireMode, this));
	(boost::bind(&SingleGroupsControlWidget::fiftyMode, this));
	(boost::bind(&SingleGroupsControlWidget::mustangMode, this));
}

void SingleGroupsControlWidget::fileTooLarge() {
}
void SingleGroupsControlWidget::fileUploaded() {

    //The uploaded filename
    std::string mFilename = upload->spoolFileName(); 

    //The file contents
    std::vector<Wt::Http::UploadedFile> mFileContents = upload->uploadedFiles();

    //The file is temporarily stored in a file with location here
    std::string mContents;
    mContents=mFileContents.data()->spoolFileName();

    Wt::log("info") << "File uploaded to server " << mContents;

    //convert file into binary
	std::ifstream in(mContents, std::ios::in | std::ios::binary);
	std::ostringstream contents;
	contents << in.rdbuf();
	in.close();
	std::string data = (contents.str());
	//Wt::log("info") << "binary data " << data;
	//convert binary file to base64 for our api
    std::string encoding = Wt::Utils::base64Encode(data);
    //Wt::log("info") << "base64 data " << encoding;
    std::string body = "{\"requests\":[{\"image\":{\"content\": \"" + encoding + "\"},\"features\":[{\"type\":\"IMAGE_PROPERTIES\"}]}]}";

	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText(body);
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVision, this, _1, _2));
	client->post("https://vision.googleapis.com/v1/images:annotate?key=AIzaSyBEC5ipSoaLDU40JqUtdeDIcIgFfy3FChA", *msg);

	
    //Do something with the contents here
    //Either read in the file or copy it to use it
    //https://www.webtoolkit.eu/wt/doc/reference/html/namespaceWt_1_1Utils.html

    //return
    return;
}
void SingleGroupsControlWidget::handleHttpResponseVision(boost::system::error_code err, const Http::Message& response) {
	Wt::log("info") << "WWOWOWOOWOWOWW" << response.body();
}

Http::Client * SingleGroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

void SingleGroupsControlWidget::handleHttpResponseUpdate(boost::system::error_code err, const Http::Message& response) {
	update();
}

void SingleGroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

void SingleGroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//get group name and lights in the group
		Json::Object result;
		Json::parse(response.body(), result);
		string name = result.get("name");
		groupName = name;
		size_t pos = response.body().find("lights");
		string subString = response.body().substr(pos + 10);
		size_t endPos = subString.find("]");
		lights = subString.substr(0, endPos);
		boost::erase_all(lights, "\"");

		//display group name and group lights
		groupInfoEdit_->setText("Group Name: " + groupName);
		groupLightsEdit_->setText("Lights in your Group: " + lights);
		removeChoices_->clear();
		addChoices_->clear();

		//give user choices to add/remove lights
		if (lights.find("1") != string::npos) {
			removeChoices_->addItem("1");
		} else {
			addChoices_->addItem("1");
		}

		if (lights.find("2") != string::npos) {
			removeChoices_->addItem("2");
		} else {
			addChoices_->addItem("2");
		}

		if (lights.find("3") != string::npos) {
			removeChoices_->addItem("3");
		} else {
			addChoices_->addItem("3");
		}

		change_->setText("");
	}
}

void SingleGroupsControlWidget::copy() {
	bool one = false;
	bool two = false;
	bool three = false;

	//get lights in the group
	if (lights.find("1") != string::npos) {
		one = true;
	}
	if (lights.find("2") != string::npos) {
		two = true;
	}
	if (lights.find("3") != string::npos) {
		three = true;
	}
	
	//create body message
	string selectedLights;
	if (one && two && three) {
		selectedLights = "[\"1\",\"2\",\"3\"]";
	} else {
		if (one && two) {
			selectedLights = "[\"1\",\"2\"]";
		}
		else {
			if (one && three) {
				selectedLights = "[\"1\",\"3\"]";
			}
			else {
				if (two && three) {
					selectedLights = "[\"2\",\"3\"]";
				}
				else {
					if (one) {
						selectedLights = "[\"1\"]";
					}
					else {
						if (two) {
							selectedLights = "[\"2\"]";
						}
						else {
							selectedLights = "[\"3\"]";
						}
					}
				}
			}
		}
	}

	//send a post request to create a new group
	change_->setText("Copy made (note: you are now still editing the original group)");
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"lights\" : " + selectedLights + ", \"name\" : \"" + groupName + "\", \"type\" : \"LightGroup\" }");
	Http::Client *client = SingleGroupsControlWidget::connect();
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->post("http://" + ip + ":" + port + "/api/" + userID + "/groups", *msg);
}

void SingleGroupsControlWidget::deleteGroup() {
	//confirm that user wants to delete the group
	if (!deleteConfirm) {
		change_->setText("You are about to delete this group. Are you sure?");
		deleteConfirm = true;
	} else {
		//delete the group and return to group page
		Http::Message *msg = new Http::Message();
		Http::Client *client = SingleGroupsControlWidget::connect();
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->deleteRequest("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
		returnBridge();
	}
}

void SingleGroupsControlWidget::addLights() {
	bool one = false;
	bool two = false;
	bool three = false;

	//if a light is selected, create a new list of lights in the group, else display an error message
	if (addChoices_->currentText() == "1" || addChoices_->currentText() == "2" || addChoices_->currentText() == "3") {
		//get lights already in the group
		if (lights.find("1") != string::npos) {
			one = true;
		}
		if (lights.find("2") != string::npos) {
			two = true;
		}
		if (lights.find("3") != string::npos) {
			three = true;
		}

		//get light that needs to be added
		if (addChoices_->currentText() == "1") {
			one = true;
		} else {
			if (addChoices_->currentText() == "2") {
				two = true;
			} else {
				three = true;
			}
		}

		//create body message (lights already in group + light that needs to be added)
		string selectedLights;
		if (one && two && three) {
			selectedLights = "[\"1\",\"2\",\"3\"]";
		} else {
			if (one && two) {
				selectedLights = "[\"1\",\"2\"]";
			} else {
				if (one && three) {
					selectedLights = "[\"1\",\"3\"]";
				} else {
					if (two && three) {
						selectedLights = "[\"2\",\"3\"]";
					} else {
						if (one) {
							selectedLights = "[\"1\"]";
						} else {
							if (two) {
								selectedLights = "[\"2\"]";
							} else {
								selectedLights = "[\"3\"]";
							}
						}
					}
				}
			}
		}

		change_->setText("Are you sure?");
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"lights\" : " + selectedLights + "}");
		Http::Client *client = SingleGroupsControlWidget::connect();
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
	} else {
		change_->setText("Please choose a light. If there are no choices then all lights are already added.");
	}
}

void SingleGroupsControlWidget::removeLights() {
	bool one = false;
	bool two = false;
	bool three = false;
	
	//if a light is selected, create the new list of lights in the groip, else dislay an error message
	if (removeChoices_->currentText() == "1" || removeChoices_->currentText() == "2" || removeChoices_->currentText() == "3") {
		//get lights already in the group
		if (lights.find("1") != string::npos) {
			one = true;
		}
		if (lights.find("2") != string::npos) {
			two = true;
		}
		if (lights.find("3") != string::npos) {
			three = true;
		}

		//get light that needs to be removed
		if (removeChoices_->currentText() == "1") {
			one = false;
		} else {
			if (removeChoices_->currentText() == "2") {
				two = false;
			} else {
				three = false;
			}
		}

		//if there is only 1 light in the group, it can't be deleted
		if (!three && !two && !one) {
			change_->setText("You must have at least 1 light in your group");
		} else {
			//create body message (lights already in group - light that needs to be removed)
			string selectedLights;
			if (one && two && three) {
				selectedLights = "[\"1\",\"2\",\"3\"]";
			} else {
				if (one && two) {
					selectedLights = "[\"1\",\"2\"]";
				} else {
					if (one && three) {
						selectedLights = "[\"1\",\"3\"]";
					} else {
						if (two && three) {
							selectedLights = "[\"2\",\"3\"]";
						} else {
							if (one) {
								selectedLights = "[\"1\"]";
							} else {
								if (two) {
									selectedLights = "[\"2\"]";
								} else {
									selectedLights = "[\"3\"]";
								}
							}
						}
					}
				}
			}
			change_->setText("Are you sure?");
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"lights\" : " + selectedLights + "}");
			Http::Client *client = SingleGroupsControlWidget::connect();
			client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
			client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
		}
	} else {
		change_->setText("Please choose a light. If there are no choices then you cannot remove any lights (groups must have at least 1 light)");
	}

}

void SingleGroupsControlWidget::name() {
	//send a put request to change group's name based on name edit textbox
	string input = nameEdit_->text().toUTF8();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"name\" : \"" + input + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
	change_->setText("Are you sure?");
}

void SingleGroupsControlWidget::on() {
	//send a put request to turn groups' light on	
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : true}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: ON");
}

void SingleGroupsControlWidget::off() {
	//send a put request to turn groups' light off
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : false}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: OFF");
}

void SingleGroupsControlWidget::hue() {
	//send a put request to change the group's hue based on hue slider
	int input = hueScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"hue\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Hue: " + to_string(input));
}

void SingleGroupsControlWidget::bright() {
	//send a put request to change the group's brightness based on brightness slider
	int input = briScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"bri\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Brightness: " + to_string(input));
}

void SingleGroupsControlWidget::sat(){
	//send a put request to change the group's saturation based on saturation slider
	int input = satScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"sat\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Saturation: " + to_string(input));
}

void SingleGroupsControlWidget::transition() {
	//send a put request to change the group's transition time based on transition slider
	int input = transitionScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"transitiontime\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Transition Time: " + to_string(input * 100) + "ms");
}

void SingleGroupsControlWidget::sunsetMode() {
	//turn the lights that are part of the group into a different shde of yellow
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"103\" , \"hue\" : \"10532\" , \"bri\" : \"211\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"59\" , \"hue\" : \"8894\" , \"bri\" : \"249\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"203\" , \"hue\" : \"10064\" , \"bri\" : \"184\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Sunset Yellow");
}

void SingleGroupsControlWidget::partyMode() {
	//create child to play music in the background
	pid_t pid = fork();
	if (pid == 0) {
		system("aplay party.wav");
		exit(0);

	//parent loops the lights
	} else {
		change_->setText("!PARTY MODE! Turn on sound for music. (Please wait until party mode completes to continue)");
		
		//change the colors 40 times through a cycle of 5 different colors 
		for (int i = 0; i < 40; i++) {
			if (i % 5 == 0) {
				if (lights.find("1") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"14043\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
				}
				if (lights.find("2") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"55237\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
				}
				if (lights.find("3") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"9596\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
				}

			}
			else {
				if (i % 4 == 0) {
					if (lights.find("1") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"49619\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
					}
					if (lights.find("2") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"19192\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
					}
					if (lights.find("3") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"42364\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
					}
				}
				else {
					if (i % 3 == 0) {
						if (lights.find("1") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"8192\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
						}
						if (lights.find("2") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"36278\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
						}
						if (lights.find("3") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"60620\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
						}
					}
					else {
						if (i % 2 == 0) {
							if (lights.find("1") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"32299\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
							}
							if (lights.find("2") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"65535\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
							}
							if (lights.find("3") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"27384\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
							}
						}
						else {
							if (lights.find("1") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"13107\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
							}
							if (lights.find("2") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"56407\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
							}
							if (lights.find("3") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"49151\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
							}
						}
					}
				}
			}
			//wait 0.25s before changing colors again
			usleep(250000);
		}
	}
	change_->setText("");
}

void SingleGroupsControlWidget::fiftyMode() {
	//turn the lights that are part of the group into a different shde of grey
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"1\" , \"hue\" : \"26214\" , \"bri\" : \"211\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"58\" , \"hue\" : \"48215\" , \"bri\" : \"82\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"0\" , \"hue\" : \"60620\" , \"bri\" : \"173\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: 50 Shades");
}

void SingleGroupsControlWidget::oceanMode() {
	//turn the lights that are part of the group into a different shde of blue
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"191\" , \"hue\" : \"33236\" , \"bri\" : \"212\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"188\" , \"hue\" : \"42364\" , \"bri\" : \"169\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"35810\" , \"bri\" : \"247\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Ocean Blue");
}

void SingleGroupsControlWidget::forestMode() {
	//turn the lights that are part of the group into a different shde of green
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"23873\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"102\" , \"hue\" : \"29023\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"121\" , \"hue\" : \"16618\" , \"bri\" : \"150\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Forest Green");
}

void SingleGroupsControlWidget::mustangMode() {
	//turn the lights that are part of the group into a different shde of purple
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"82\" , \"hue\" : \"45874\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"152\" , \"hue\" : \"51726\" , \"bri\" : \"109\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"50322\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Mustang Purple");
}

void SingleGroupsControlWidget::fireMode() {
	//turn the lights that are part of the group into a different shde of orange
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"7256\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"227\" , \"hue\" : \"3511\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"147\" , \"hue\" : \"7490\" , \"bri\" : \"212\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Fire Orange");
}

void SingleGroupsControlWidget::bloodMode() {
	//turn the lights that are part of the group into a different shde of red
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"132\" , \"hue\" : \"0\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"3511\" , \"bri\" : \"161\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"63663\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Blood Red");
}

void SingleGroupsControlWidget::returnBridge(){
	//go to /bridge for BridgeControlWidget
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}


/*

https://vision.googleapis.com/v1/images:annotate
https://vision.googleapis.com/v1/images:annotate?key=AIzaSyBEC5ipSoaLDU40JqUtdeDIcIgFfy3FChA

AIzaSyD9vkifoS9ag2w0z1NPiZIlo3IF5FWzWww


*/