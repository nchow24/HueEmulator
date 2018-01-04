/**
* @file BridgeControl.C
* @Author Daniel Le
* @date November 27, 2017
* @brief Page for choosing a bridge and registering one
*
* Creates a page that allows the user to select a bridge from a list of registered bridges.
* Also displays a form that allows the user to enter a bridge's name, IP address, port number, and location
* The input is checked to make sure a number is entered for the port number and the selected port isn't already taken
*
**/
#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>
#include <Wt/WSound>



#ifndef BRIDGECONTROL_H_
#define BRIDGECONTROL_H_

class Session;

class BridgeControlWidget : public Wt::WContainerWidget
{
public:
	BridgeControlWidget(Session *session, WContainerWidget *parent);

	/**
	* @brief Refreshes the page information
	*
	* Refreshes the page elements and clears the form. Loads in the new bridge into the list of registered bridges if one was created
	* @return Void.
	*
	**/
	void update();

private:
	Session *session_;

	Wt::WText *ip_;
	Wt::WLineEdit *ipEdit_;				/*!< text input for the bridge's IP address */
	Wt::WText *port_;					
	Wt::WLineEdit *portEdit_;			/*!< text input for the bridge's port number */
	Wt::WText *location_;
	Wt::WLineEdit *locationEdit_;		/*!< text input for the bridge's location */
	Wt::WLineEdit *name_;				/*!< text input for the bridge's name */
	Wt::WPushButton *button;			/*!< button to register the bridge */
	Wt::WText *errorText_;				/*!< textbox displaying indications of an error*/
	std::string username;				/*!< string storing the bridge user id*/
	std::string ip;						/*!< string storing the user's IP address*/
	std::string port;					/*!< string storing the bridge's port number*/
	Wt::WText *confirm_;				/*!< textbox asking for confirmation from the user when they want to register a bridge*/

	/**
	* @brief Handles POST response and adds bridge to the database
	*
	* Receives the response after a POST call to the server to register a user with the new bridge
	* First checks if an error was returned from the server because the bridge was not linked yet
	* If the POST call was successful, the username is retrieved from the response's JSON string
	* The bridge's name, IP address, port number, and location are retrieved from the input form
	* The new bridge will be created with the new information before being placed into the database
	*
	* @param err Error code indicating if an error occurrred
	* @param response Message containing the response from the server
	* @return Void.
	**/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);

	/**
	* @brief creates an HTTP client
	*
	* The client is used to make POST calls to the bridge in order to register the user to the bridge and using the returned username to add the bridge to the database
	* @return Http::Client
	*/
	Wt::Http::Client * connect();

	/**
	* @brief Checks if the input is valid
	*
	* First checks to make sure the port number has received a number as input. Then checks if the given port number is already in use.
	* If either of these are false, displays an error message for the corresponding error. If there are no errors, it makes a POST call to the server
	* which sets the new bridge as registered to the user
	* @return Void.
	**/
	void registerBridge();

	/**
	* @brief Redirects to a page with the bridge's lights
	*
	* Switches to a page with a bridge's lights. The selected bridge is determined by the ip and port that is passed
	* @return Void.
	**/
	void showLights();
	
	 Wt::WSound *messageReceived_;
};


#endif