/** @file RegistrationView.C
*  @brief Creates a registration view for the user to interact with
*
* Creates a custom registration view that is build ontop of the auth::registrationView.
* This allows us to add UserDetailsModel onto the pre-existing registration view to take
* on form data for the first and last name to be stored.
*
*  @author Paul Li
*  @date Nov 28, 2017
*/
#include "RegistrationView.h"
#include "UserDetailsModel.h"

#include <Wt/WLineEdit>


/** @brief Constructor for RegistrationView
 *
 *  Set the template/view of the registration section on our website and the model
 *  of the first name and last name section from UserDetailsModel. 
 *  
 *  @param session is the current session data of the app.
 *  @param authWidget is the information of the authWidget 
 */
RegistrationView::RegistrationView(Session& session,
				   Wt::Auth::AuthWidget *authWidget)
  : Wt::Auth::RegistrationWidget(authWidget),
    session_(session)
{
  setTemplateText(tr("template.registration"));
  detailsModel_ = new UserDetailsModel(session_, this);

  updateView(detailsModel_);
}
/** @brief Creates the WWidgets that will be displayed on the website
 *
 *  Creates the form widgets by adding WWidgets and utilizing the templates.xml file to 
 *  help draw and display the form information to the user. A function
 *  adds onto to the preexiting Auth::registrationWidget to allow 2 additional WLineEdits.
 *  
 *  @param field is the WFormModel::Field that will create the form widget itself.
 */
Wt::WWidget *RegistrationView::createFormWidget(Wt::WFormModel::Field field)
{
  if (field == UserDetailsModel::FirstNameField || field == UserDetailsModel::LastNameField )
    return new Wt::WLineEdit();
  else
    return Wt::Auth::RegistrationWidget::createFormWidget(field);
}
/** @brief Checks the information in the form if it is valid.
 *
 *  Validates the information passed in the form and checks if everything is correct and passes validations before
 *  sending in the form to the server.
 *  
 *  @return returns the results of the registration validation.
 */
bool RegistrationView::validate()
{
  bool result = Wt::Auth::RegistrationWidget::validate();

  updateModel(detailsModel_);
  if (!detailsModel_->validate())
    result = false;
  updateView(detailsModel_);

  return result;
}
/** @brief Checks the information in the form if it is valid.
 *
 *  Validates the information passed in the form and checks if everything is correct and passes validations before
 *  sending in the form to the server.
 *  
 *  @param user the auth model of the user logged in.
 */
void RegistrationView::registerUserDetails(Wt::Auth::User& user)
{
  detailsModel_->save(user);
}
