/** @file LightsControl.h
*  @class LightsControl
*  @brief Function prototypes for the LightsControl class that manages the states and changes of states in lights
*  @author Nicole Chow
*  @date Nov 28, 2017
*/

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_

class Session;

class LightsControlWidget: public Wt::WContainerWidget
{
public:

  /** @brief creates a LightsControlWidget
  *
  *  creates a LightsControlWidget that is tied to a database session. The Widget also has a parent Container Widget
  *
  *  @param session the database session
  *  @param parent the parent Widget Container
  *  @return LightsControlWidget
  */
  LightsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);									
  
  /** @brief loads LightsControlWidget page
  *
  *  loads the LightsControlWidget page which displays changes to a light's name, and options to change a light's state
  *
  *  @return Void
  */
  void update();												

private:
	Session *session_;									/*!< keeps track of light status */
	std::string currentLight = "0";						/*!< the light that is currently being changed */
	std::string ip = "";								/*!< bridge's IP address */
	std::string userID = "";							/*!< user's bridge ID */
	std::string port = "";								/*!< bridge's port number */
	std::string customHue;								/*!< custom hue value */
	std::string customSat;								/*!< custom saturation value */
	std::string customBri;								/*!< custom brightness value */
	Wt::WLineEdit *nameEdit_;							/*!< light's name to be changed */
	Wt::WSlider *satScaleSlider_;						/*!< light's saturation selection */
	Wt::WSlider *briScaleSlider_;						/*!< light's brightness selection */
	Wt::WSlider *hueScaleSlider_;						/*!< light's hue selection */
	Wt::WSlider *transitionScaleSlider_;				/*!< light's transition time selection */
	Wt::WText *oneLight_;								/*!< light 1's name */
	Wt::WText *twoLight_;								/*!< light 2's name */
	Wt::WText *threeLight_;								/*!< light 3's name */
	Wt::WText *change_;									/*!< status of a light change */
	Wt::WText *light_;									/*!< displays the light being changed */
	
	/** @brief creates an HTTP client
	*
	*  creates an HTTP client that will be used to send either a put request change a light or a get request to retrive a light's state
	*
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();

	/** @brief turns a light on
	*
	*  turns a light on. A light must be selected first.
	*
	*  @return Void
	*/
	void on();

	/** @brief turns a light off
	*
	*  turns a light off. A light must be selected first.
	*
	*  @return Void
	*/
	void off();	

	/** @brief changes a light's hue
	*
	*  changes a light's hue based on the hue value. A light must be selected first.
	*
	*  @return Void
	*/
	void hue();	

	/** @brief changes a light's brightness
	*
	*  changes a light's brightness based on the brightness slider value. A light must be selected first.
	*
	*  @return Void
	*/
	void bright();	

	/** @brief changes a light's saturation
	*
	*  changes a light's saturation based on the saturation slider value. A light must be selected first.
	*
	*  @return Void
	*/
	void sat();	

	/** @brief changes a light's name
	*
	*  changes a light's name. A light must be selected first.
	*
	*  @return Void
	*/
	void name();

	/** @brief changes a light's transition time
	*
	*  changes a light's transition time based on the transition slider value. A light must be selected first.
	*
	*  @return Void
	*/
	void transition();	

	/** @brief selects light 1 to change
	*
	*  selects light 1 such that any changes in state will be applied to light 1
	*
	*  @return Void
	*/
	void lightOne();

	/** @brief selects light 2 to change
	*
	*  selects light 2 such that any changes in state will be applied to light 2
	*
	*  @return Void
	*/
	void lightTwo();

	/** @brief selects light 3 to change
	*
	*  selects light 3 such that any changes in state will be applied to light 3
	*
	*  @return Void
	*/
	void lightThree();

	/** @brief returns user to the bridge page
	*
	*  returns user the bridge page by setting the internal path
	*
	*  @return Void
	*/
	void returnBridge();

	/** @brief deletes a bridge
	*
	*  removes a bridge from session and returns user back to the bridge page
	*
	*  @return Void
	*/
	void deleteBridge();

	/** @brief creates a custom light mode
	*
	*  takes the chosen hue, saturation and brightness values from the sliders and creates a custom mode button. If custom moe already exits, new custom mode overrides the old one.
	*
	*  @return Void
	*/
	void customCreate();

	/** @brief implementss acustom light mode
	*
	*  changes a light to the hue, saturation and brightness in the custom mode
	*
	*  @return Void
	*/
	void customMode();

	/** @brief handles response and displays light information
	*
	*  gets the current state of a light and adjusts the slider values to reflect the light's state. Called when a user chooses a light to change
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	
	/** @@brief handles response for light name changes
	*
	*  uses update() to refresh the page with changes to light names. Called whea user change the name of a light
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseName(boost::system::error_code err, const Wt::Http::Message& response);
	
	/** @brief handles response and does nothing
	*
	*  does nothing. Used for functions where the page does not need to be refreshed nor does light information need to be fetched again.
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
};

#endif
