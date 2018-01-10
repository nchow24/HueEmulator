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
#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>
#include "Bridge.h"


#ifndef BRIDGEEDITCONTROL_H_
#define BRIDGEEDITCONTROL_H_

class Session;

class BridgeEditControlWidget : public Wt::WContainerWidget
{
public:

	/**
	* @brief Constructor
	*
	* Creates the instance of the bridge editing page to load in the information and widgets
	*
	* @param session The database containing bridge information
	* @param parent The view where visual elements will be rendered in
	**/
	BridgeEditControlWidget(Session *session, Wt::WContainerWidget *parent = 0);

	/**
	* @brief Updates page
	*
	* Reloads the page to refresh the list of existing bridges and reset the form for entering
	* bridge name, IP address, port number, and location
	* @return Void.
	**/
	void update();

private:
	Session *session_;						/*!< keeps track of bridge information */
	Bridge *thisBridge;						/*!< represents the current bridge */
	Wt::WLineEdit *nameEdit_;				/*!< text input for the bridge's name*/
	Wt::WLineEdit *ipEdit_;					/*!< text input for the bridge's IP address*/
	Wt::WLineEdit *portEdit_;				/*!< text input for the bridge's port number*/
	Wt::WLineEdit *locationEdit_;			/*!< text input for the bridge's location*/
	Wt::WText *errorText_;					/*!< textbox displaying any indication of an error*/
	std::string userId ="";					/*!< current bridge ID*/
	std::string ip = "";					/*!< current bridge IP address*/
	std::string port = "";					/*!< current bridge port number*/

	/**
	* @brief Checks user input and then sends a POST request
	*
	* Checks if the port number is an integer and if the entered port number is not taken already. If either are false, an error text is displayed.
	* With the given information from the user, makes a POST request to connect to a new or the existing bridge. 
	* @return Void.
	**/
	void updateEdit();			

	/**
	* @brief Handles POST response and updates bridge in the database
	*
	* Receives the response after a POST call to the server to register a user with the new or existing bridge
	* First checks if an error was returned from the server because the bridge was not linked yet
	* If the POST call was successful, the username is retrieved from the response's JSON string
	* The bridge's name, IP address, port number, and location are retrieved from the input form
	* The original bridge will be deleted and then recreated with the new information before being placed into the database
	*
	* @param err Error code indicating if an error occurrred
	* @param response Message containing the response from the server
	* @return Void.
	**/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);

	/** 
	* @brief creates an HTTP client
	* 
	* The client is used to make POST calls to the bridge in order to register the user to the bridge and using the returned username to update the bridge in the database
	* @return Http::Client
	*/
	Wt::Http::Client * connect();
};

#endif
