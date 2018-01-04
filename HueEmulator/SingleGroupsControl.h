/** @file SingleGroupsControl.h
*  @class SingleGroupsControl
*  @brief Function prototypes for the SingleGroupsControl class that manages the deletion, states and changes of states in groups
*  
*  allows user to alter the hue, brightness, on/off, saturation, transition time, and name of a specific group. User can also add/remove lights or delete the group.
*  
*  @author Nicole Chow
*  @date Nov 28, 2017
*/

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef SINGLEGROUPCONTROL_H_
#define SINGLEGROUPCONTROL_H_

class Session;

class SingleGroupsControlWidget: public Wt::WContainerWidget
{
public:

	/** @brief creates a SingleGroupsControlWidget
	*
	*  creates a SingleGroupsControlWidget that is tied to a database session. The Widget also has a parent Container Widget
	*
	*  @param session the database session
	*  @param parent the parent Widget Container
	*  @return SingleGroupsControlWidget
	*/
	SingleGroupsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	
	/** @brief loads SingleGroupsControlWidget page
	*
	*  loads the SingleGroupsControlWidget page which displays changes to the group name, lights in the group and options of lights to add/remove
	*
	*  @return Void
	*/
	void update();

private:
	Session *session_;												/*!< keeps track of group status */
	std::string groupName = "";										/*!< name of the group */
	std::string ip = "";											/*!< bridge's IP address */
	std::string userID = "";										/*!< user's bridge ID */
	std::string port = "";											/*!< bridge's port number */
	std::string groupID = "";										/*!< group's ID */
	std::string lights;												/*!< group's lights */
	bool deleteConfirm;												/*!< confirmation of intent to delete group */		
	Wt::WLineEdit *nameEdit_;										/*!< groups' name to be changed */
	Wt::WText *groupInfoEdit_;										/*!< group name display */
	Wt::WText *groupLightsEdit_;									/*!< group's light display */
	Wt::WSlider *satScaleSlider_;									/*!< group's saturation to change into */
	Wt::WSlider *briScaleSlider_;									/*!< group's brightness to change into */
	Wt::WSlider *hueScaleSlider_;									/*!< group's hue to change into */								
	Wt::WSlider *transitionScaleSlider_;							/*!< group's transition time to change into */
	Wt::WText *change_;												/*!< status of a group change */
	Wt::WComboBox *addChoices_;										/*!< selected light to add to group */
	Wt::WComboBox *removeChoices_;									/*!< selected light to remove from group */
	Wt::WFileUpload *upload;
	
	/** @brief creates an HTTP client
	*
	*  creates an HTTP client that will be used to send either a put request to change the state of a group or a get request to obtain information about the group name/light members
	*
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();

	/** @brief plays music in background
	*
	*  plays a .wav file in the background. Used for party mode.
	*
	*  @param sound audio file
	*  @return Void
	*/
	static void music(Wt::WSound *sound);

	/** @brief turns group's lights on
	*
	*  turns all the lights that are in the group on. If the lights are already on then there is no change
	*
	*  @return Void
	*/
	void on();

	/** @brief turns group's lights off
	*
	*  turns all the lights that are in the group off. If the lights are already off then there is no change
	*
	*  @return Void
	*/
	void off();

	/** @brief changes group's hue
	*
	*  changes all the lights that are in the group to the same hue that is specified on the hue slider
	*
	*  @return Void
	*/
	void hue();

	/** @brief changes group's brightness
	*
	*  changes all the lights that are in the group to the same brightness that is specified on the brightness slider
	*
	*  @return Void
	*/
	void bright();

	/** @brief changes group's saturation
	*
	*  changes all the lights that are in the group to the same saturation that is specified on the saturation slider
	*
	*  @return Void
	*/
	void sat();

	/** @brief changes group's name
	*
	*  changes the groups name. Changes are displayed afterwards using update()
	*
	*  @return Void
	*/
	void name();

	/** @brief changes group's transition time
	*
	*  changes all the lights that are in the group to the same transition time that is specified on the transition slider
	*
	*  @return Void
	*/
	void transition();

	/** @brief adds light to group
	*
	*  takes input from a dropdown list of available lights to add to the group. If there are no more lights that can be added then the dropdown list is empty
	*
	*  @return Void
	*/
	void addLights();

	/** @brief removes light from group
	*
	*  takes input from a dropdown list of available lights to remove from the group. If there is only 1 light in the group it cannot be removed.
	*
	*  @return Void
	*/
	void removeLights();

	/** @brief puts group on party mode for 10s (color looping). Must wait for party mode to finish. 
	*
	*  loops the lights in the group through every 0.25s for 10s. Party.wav plays in the background using music(). User must wait for party mode to finish in order to continue using the server
	*
	*  @return Void
	*/
	void partyMode();

	/** @brief creates a copy of the current group
	*
	*  gets the name and current lights in the group and creates a copy using post request
	*
	*  @return Void
	*/
	void copy();

	/** @brief puts group on sunset mode (yellows)
	*
	*  turns on all the lights in the group and changes them to a different shade of yellow
	*
	*  @return Void
	*/
	void sunsetMode();

	/** @brief puts group on fifty mode (greys)
	*
	*  turns on all the lights in the group and changes them to a different shade of grey
	*
	*  @return Void
	*/
	void fiftyMode();


	/** @brief puts group on mustang mode (purples)
	*
	*  turns on all the lights in the group and changes them to a different shade of purple
	*
	*  @return Void
	*/
	void mustangMode();


	/** @brief puts group on blood mode (reds)
	*
	*  turns on all the lights in the group and changes them to a different shade of red
	*
	*  @return Void
	*/
	void bloodMode();


	/** @brief puts group on ocean mode (blues)
	*
	*  turns on all the lights in the group and changes them to a different shade of blue
	*
	*  @return Void
	*/
	void oceanMode();


	/** @brief puts group on fire mode (oranges)
	*
	*  turns on all the lights in the group and changes them to a different shade of orange
	*
	*  @return Void
	*/
	void fireMode();


	/** @brief puts group on forest mode (greens)
	*
	*  turns on all the lights in the group and changes them to a different shade of green
	*
	*  @return Void
	*/
	void forestMode();


	/** @brief deletes a group
	*
	*  deletes the group and returns user back to the bridge page using returnBridge(). The group will no longer be listed as existing and cannot be accessed
	*
	*  @return Void
	*/
	void deleteGroup();


	/** @brief returns user to bridge page
	*
	* returns user the bridge page by setting the internal path
	*
	*  @return Void
	*/
	void returnBridge();

	/** @brief handles response and displays group information
	*
	*  gets the group's name and the lights in the group and displays them
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief handles response reloads page
	*
	*  uses update() to refresh the page with changes to group states
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief handles response and does nothing
	*
	*  does nothing. Used for functions where the page does not need to be refreshed nor does group information need to be fetched again.
	*
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err /*!< error code */, const Wt::Http::Message& response /*!< response */);

	void handleHttpResponseVision(boost::system::error_code err, const Wt::Http::Message& response);
	void fileTooLarge();
	void fileUploaded();
};

#endif
