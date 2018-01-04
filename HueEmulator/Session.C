/** @file Session.C
*  @brief Session handling for the application
*
*   Handles the Users who are able to access the database and create a
*   persistent environment for our application using sqlite3. It hashes
*   user passwords and stores tokens. The is also a multitude of functions
*   to help access and store information into the applications database. 
*   Through out the app session is used to authorize each user to a page.
*
*  @author Paul Li
*  @date Nov 28, 2017
*/

#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"
#include "Wt/Auth/GoogleService"
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"

#include <Wt/WApplication>
#include <Wt/WLogger>

#include "Session.h"


#ifndef WT_WIN32
#include <unistd.h>
#endif
//check if crypting is avalible
#if !defined(WT_WIN32) && !defined(__CYGWIN__) && !defined(ANDROID)
#define HAVE_CRYPT
#endif

using namespace Wt;
namespace dbo = Wt::Dbo;

namespace {

/** @brief Used to compute a password hash that can be used to store or verify user account passwords.
 */
#ifdef HAVE_CRYPT
class UnixCryptHashFunction : public Auth::HashFunction
  {
  public:
    virtual std::string compute(const std::string& msg, 
				const std::string& salt) const
    {
      std::string md5Salt = "$1$" + salt;
      return crypt(msg.c_str(), md5Salt.c_str());
    }

    virtual bool verify(const std::string& msg,
			const std::string& salt,
			const std::string& hash) const
    {
      return crypt(msg.c_str(), hash.c_str()) == hash;
    }

    virtual std::string name () const {
      return "crypt";
    }
  };
#endif // HAVE_CRYPT

  class MyOAuth : public std::vector<const Auth::OAuthService *>
  {
  public:
    ~MyOAuth()
    {
      for (unsigned i = 0; i < size(); ++i)
	   delete (*this)[i];
    }
  };

  Auth::AuthService myAuthService;
  Auth::PasswordService myPasswordService(myAuthService);
  MyOAuth myOAuthServices;
}
/** @brief Configure the authorization services for the app
 *
 *  Using Wt's built in authorization set the model of the services that were used
 *  such as, remember me tokens, email verifications, password hashing and etc
 */
void Session::configureAuth()
{
  myAuthService.setAuthTokensEnabled(true, "hueappcookie");
  myAuthService.setEmailVerificationEnabled(true);
  myAuthService.setIdentityPolicy(Wt::Auth::IdentityPolicy::EmailAddressIdentity);

  Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
  verifier->addHashFunction(new Auth::BCryptHashFunction(7));

#ifdef HAVE_CRYPT
  verifier->addHashFunction(new UnixCryptHashFunction());
#endif

  myPasswordService.setVerifier(verifier);
  Auth::PasswordStrengthValidator *strengthValid = new Auth::PasswordStrengthValidator();
  strengthValid->setMinimumLength(Wt::Auth::PasswordStrengthValidator::PasswordType::TwoCharClass, 8);
  myPasswordService.setStrengthValidator(strengthValid);
  myPasswordService.setAttemptThrottlingEnabled(true);

  if (Auth::GoogleService::configured())
    myOAuthServices.push_back(new Auth::GoogleService(myAuthService));
}

/** @brief Constructor of the Session
 *
 *  The constructor session is called by the HueApp to do a multitude of things
 *  - create the database if it doesn't exist
 *  - connect to the database
 *  - create the tables in the database
 *  - add a few records to the database
 *  - log a few things regarding the database
 */
Session::Session()
  : sqlite3_(WApplication::instance()->appRoot() + "hueApp.db")
{
  session_.setConnection(sqlite3_);
  sqlite3_.setProperty("show-queries", "true");
  session_.mapClass<User>("user");
  session_.mapClass<Bridge>("bridge");
  session_.mapClass<BridgeUserIds>("BridgeUserIds");
  session_.mapClass<AuthInfo>("auth_info");
  session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
  session_.mapClass<AuthInfo::AuthTokenType>("auth_token");
  

  users_ = new UserDatabase(session_);

  dbo::Transaction transaction(session_);
  try {
    session_.createTables();
    /*
     * Add a default guest/guest account
     */
    Auth::User guestUser = users_->registerNew();
    guestUser.addIdentity(Auth::Identity::LoginName, "guest");
    myPasswordService.updatePassword(guestUser, "guest");

    Wt::log("info") << "Database created";
  } catch (...) {
    Wt::log("info") << "Using existing database";
  }

  transaction.commit();
}


/** @brief Deconstructor of the Session
 *
 *  Deallocates the memory of users_
 */
Session::~Session()
{
  delete users_;
}
/** @brief Get the database object of User of the currently logged in user.
 *
 *  A helper function that is used to get the currently logged in user for other session functions to easily 
 *  find and user.
 *  
 *  @return the database object of User of the currently logged in user.
 */
dbo::ptr<User> Session::user()
{
  if (login_.loggedIn()) {
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    dbo::ptr<User> user = authInfo->user();

    if (!user) {
      user = session_.add(new User());
      authInfo.modify()->setUser(user);
    }
    return user;
  } else
    return dbo::ptr<User>();
}
/** @brief Get the database object of User of the currently logged in user.
 *
 *  A helper function that is used to get the currently logged in user for other session functions to easily 
 *  find and user. An overloaded function to allow the authUser configuration to be passed in.
 *  
 *  @param authUser configuration of the user.
 *  @return the database object of User of the currently logged in user.
 */
dbo::ptr<User> Session::user(const Wt::Auth::User& authUser)
{
  dbo::ptr<AuthInfo> authInfo = users_->find(authUser);

  dbo::ptr<User> user = authInfo->user();

  if (!user) {
    user = session_.add(new User());
    authInfo.modify()->setUser(user);
  }

  return user;
}

/** @brief Get the username of the currently logged in user.
 *
 *  Gets the username/email of the currently logged in user that was created during the registration page.
 *  depending on what the model was set for the Auth service.
 *  
 *  @return the string of the username of the currently logged in user.
 */
std::string Session::userName() const
{
  if (login_.loggedIn())
    return login_.user().identity(Auth::Identity::LoginName).toUTF8();
  else
    return std::string();
}

/** @brief Get the first name of the currently logged in user.
 *
 *  Gets the first name of the currently logged in user that was created during the registration page.
 *  
 *  @return the string of the first name of the currently logged in user.
 */
std::string Session::firstName()
{
  User* user = this->getUser();
  return user->firstName;
}

/** @brief Get the last name of the currently logged in user.
 *
 *  Gets the last name of the currently logged in user that was created during the registration page.
 *  
 *  @return the string of the last name of the currently logged in user.
 */
std::string Session::lastName()
{
  User* user = this->getUser();
  return user->lastName;
}

/** @brief Get a bridge from the database.
 *
 *  Finds and returns the bridge from the Bridge database by searching for the ip and port of the desired bridge.
 *  
 *  @param ip of the bridge to be retrived.
 *  @param port of the bridge to be retrived.
 *  @return a bridge object of found Bridge.
 */
Bridge* Session::getBridge(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  transaction.commit();
  return bridgeObj.modify();
}

/** @brief Removes a bridge from the database.
 *
 *  Finds and removes the bridge from the Bridge database by searching for the ip and port of the desired bridge.
 *  
 *  @param ip of the bridge of interest to be deleted.
 *  @param port of the bridge of interest to be deleted.
 *  @return if deleting the bridge was successful or not.
 */
bool Session::deleteBridge(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  if(bridgeObj){
    bridgeObj.remove();
    transaction.commit();
    return true;
  }else{
    Wt::log("info") << "Failed to delete could not find given bridge. ("
                    << ip <<":"<<port << ")";
    transaction.commit();
    return false;
  }
}

/** @brief Updates one of the pre-existing bridges in our database.
 *
 *  Finds and updates a bridge that is in our database. It is found in our database with the oldBridge object
 *  to get the ip and port. The newBridge object is used replace the data of the oldBridge data in the db.
 *  
 *  @param oldBridge is the bridge in the database to be replaced.
 *  @param newBridge is the new data to replace the oldBridge.
 */
void Session::updateBridge(Bridge* oldBridge, Bridge* newBridge){
  dbo::Transaction transaction(session_);
  Wt::log("info") << "Bridge being updated" << newBridge->getIpAddress() << ":" << newBridge->getPortNumber() ;
  
  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
                .where("ipAddress = ?").bind(oldBridge->getIpAddress())
                .where("portNumber = ?").bind(std::to_string(oldBridge->getPortNumber()));
	  bridgeObj.modify()->setBridgeName(newBridge->getBridgeName());
	  bridgeObj.modify()->setLocation(newBridge->getLocation());
	  bridgeObj.modify()->setIpAddress(newBridge->getIpAddress());
	  bridgeObj.modify()->setUserId(newBridge->getUserId());
	  bridgeObj.modify()->setRegistered(newBridge->getRegistered());
	  bridgeObj.modify()->setPortNumber(newBridge->getPortNumber());
 
  transaction.commit();
}

/** @brief Gets a vector list of all the bridges in the database.
 *
 *  Gets a vector list of all the bridges in the database of all the bridges.
 *  
 *  @return a vector of Bridge objects
 */
std::vector<Bridge> Session::getAllBridges(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgePtr> query = session_.find<Bridge>();
  Bridges_Collection bridges = query.resultList();
  std::vector<Bridge> x;
  for (Bridges_Collection::const_iterator i = bridges.begin(); i != bridges.end(); ++i){
    dbo::ptr<Bridge> bridge = *i;
    x.push_back(*bridge);
  }

  transaction.commit();
  return x;
}

/** @brief Gets a vector list of the bridges in the database of currently logged in user.
 *
 *  Gets a vector list of all the bridges in the database of all the bridges associated with the currently logged in user.
 *  
 *  @return a vector of Bridge objects.
 */
std::vector<Bridge> Session::getBridges(){
  dbo::Transaction transaction(session_);


  dbo::ptr<User> u = user();
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>().where("userID_id = ?").bind(u.id());
  BridgeUserIds_Collection ids = query.resultList();
  std::vector<Bridge> x;

  for (BridgeUserIds_Collection::const_iterator i = ids.begin(); i != ids.end(); ++i){
    BridgeUserIds_Ptr temp = *i;
    BridgePtr bridgeObj = session_.find<Bridge>().where("id = ?").bind(temp->bridge.id());
    x.push_back(*bridgeObj);
  }

  transaction.commit();
  return x;
}

/** @brief Add bridge to the database
 *
 *  Adds a new Bridge to the database by passing in a Bridge object.
 *  
 *  @param newBridge the bridge that is needed to be added to the database.
 *  @return if it was successfully added to the database
 */
bool Session::addBridge(Bridge* newBridge){
  
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj;
    bridgeObj = session_.find<Bridge>().where("ipAddress = ?").bind(newBridge->getIpAddress())
                                        .where("portNumber = ?").bind(newBridge->getPortNumber());
    if(!bridgeObj){
      bridgeObj = session_.add(newBridge);
      transaction.commit();
      return true;
    }else{
      transaction.commit();
      return false;
    }

  transaction.commit();
}

/** @brief Update the currently logged in user with the data in newUser.
 *
 *  Update the currently logged in user with the new User object that is passed in through the parameters.
 *  
 *  @param newUser a User object that holds the new data.
 */
void Session::updateUser(User* newUser){
  dbo::Transaction transaction(session_);

  dbo::ptr<User> user = this->user();
  user.modify()->name = newUser->name;
  user.modify()->firstName = newUser->firstName;
  user.modify()->lastName = newUser->lastName;
  user.modify()->email = newUser->email;
  user.modify()->customMode = newUser->customMode;

  transaction.commit();
}

/** @brief Get a User object of the currently logged in user.
 *
 *  Get the record from the database of the currently logged in user.
 *  
 *  @return a User object of the currently logged in user
 */
User* Session::getUser(){
  dbo::Transaction transaction(session_);
  dbo::ptr<User> user = session_.find<User>()
            .where("id = ?").bind(this->user().id());
  transaction.commit();
  return user.modify();
}

  //-------------------------------------
  //---------Bridge With Users DB--------
  //-------------------------------------

/** @brief Add a BridgeUserId to the database.
 *
 *  Add a BridgeUserId record to the BridgeUserIds db by passing in data (bridgeUserId) and the associated
 *  items newBridge. When creating the record it associated the User with the currently logged in user.
 *  
 *  @param newBridge the bridge associated with the UserId.
 *  @param bridgeUserId the UserId to access the bridge. 
 */
void Session::addBridgeUserId(Bridge *newBridge, std::string bridgeUserId){
  dbo::Transaction transaction(session_);
  
  // check if bridge exists
  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(newBridge->getIpAddress())
            .where("portNumber = ?").bind(newBridge->getPortNumber());
  if(!bridgeObj){
    Wt::log("info") << "Adding BridgeUserId failed. Bridge doesnt' exist";
    transaction.commit();
    return;
  }
  // check there are no duplicates
  int count = session_.query<int>("select count(1) from BridgeUserIds")
                    .where("bridgeID_id = ?").bind(bridgeObj.id())
                    .where("userID_id = ?").bind(this->user().id());

  if(count > 0){
    Wt::log("info") << "Adding BridgeUserId failed. BridgeUserId already exists!";
    transaction.commit();
    return;
  }

  BridgeUserIds *temp = new BridgeUserIds(user(), bridgeObj,bridgeUserId);
  BridgeUserIds_Ptr x = session_.add(temp);

  transaction.commit();
}

//======GETTERS======

/** @brief Gets a vector list of all the BridgeUserIds of the currently logged in user in the database.
 *
 *  Function gets all the records from the database that belong to the currently logged in user. The function returns
 *  a list of BridgeUserIds objects that can be used to fetch the userId to be used by the bridges.
 *  
 *  @return Vector list of BridgeUserIds objects in the database.
 */
std::vector<BridgeUserIds> Session::getBridgeUserId(){
  dbo::Transaction transaction(session_);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("userID_id = ?").bind(this->user().id());
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }
  transaction.commit();
  return x;
}

/** @brief Gets the BridgeUserIds record of the currently logged in user in the database and belong to bridge with ip + port.
 *
 *  getBridgeUserId is used to help get a specific record from the database of a currently logged in user by specifying the ip and 
 *  port of the bridge in the db associated with the record.
 *
 *  @param ip is used to get the bridge id to find the record in BridgeUserId.
 *  @param port is used to get the bridge id to find the record in BridgeUserId.
 *  @return BridgeUserId objects in the database.
 */
BridgeUserIds* Session::getBridgeUserId(std::string ip, std::string port){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId obtained: "<< y.modify()->bridgeUserID;
                            
  transaction.commit();
  return y.modify();
}

/** @brief Gets the BridgeUserIds record of the currently logged in user in the database and belong to bridge object bridgeObj.
 *
 *  getBridgeUserId is used to help get a specific record from the database of a currently logged in user by specifying the bridge 
 *  object passed in to find the bridge in the db associated with the record.
 *
 *  @param bridgeObj the bridge to search for in the database.
 *  @return BridgeUserId objects in the database.
 */
BridgeUserIds* Session::getBridgeUserId(Bridge *bridgeObj){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId obtained: "<< y.modify()->bridgeUserID;

  transaction.commit();
  return y.modify();
}
/** @brief Gets a vector list of all the BridgeUserIds in the database.
 *
 *  Get a list of BridgeUserIds objects of all the records in the database
 *  even if it doesn't belong to the currently logged in user.
 *
 *  @return Vector list of all the BridgeUserIds in the database.
 */
std::vector<BridgeUserIds> Session::getAllBridgeUserId(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>();
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;
  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }

  transaction.commit();
  return x;
}
/** @brief Gets a vector list of all the BridgeUserIds in the database with a bridge of ip + port
 *
 *  Get a list of BridgeUserIds objects of all the records in the database that belong to the bridge 
 *  with ip and port, even if it doesn't belong to the currently logged in user.
 *
 *  @param ip is used to find bridge id that will be removed.
 *  @param port is used to find bridge id that will be removed.
 *  @return Vector list of all the BridgeUserIds in the database that belong to bridge id of ip + port.
 */
std::vector<BridgeUserIds> Session::getAllBridgeUserId(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }
  transaction.commit();
  return x;
}
/** @brief Gets a vector list of all the BridgeUserIds in the database with a bridge of bridgeObj.
 *
 *  Get a list of BridgeUserIds objects of all the records in the database that have the Bridge, bridgeObj
 *  even if it doesn't belong to the currently logged in user.
 * 
 *  @param bridgeObj Remove all BridgeUserIds with bridgeObj.
 *  @return Vector list of all the BridgeUserIds in the database that belong to bridgeObj.
 */
std::vector<BridgeUserIds> Session::getAllBridgeUserId(Bridge *bridgeObj){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }
  transaction.commit();
  return x;
}
/** @brief Update a BridgeUserID's userid of a currently logged in user with ip and port
 *
 *  Make changes to a record in the database
 *
 *  @param ip is used to get the bridge id to be updated from BridgeUserId.
 *  @param port is used to get the bridge id to be updated from BridgeUserId.
 *  @param newBridgeUserId is the new data to be updated to bridgeUserID.
 */
void Session::updateBridgeUserId(std::string ip, std::string port, std::string newBridgeUserId){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId obtained: "<< y.modify()->bridgeUserID;

  y.modify()->bridgeUserID = newBridgeUserId;

  Wt::log("info") << "New BridgeUserId: "<< y.modify()->bridgeUserID;
                            
  transaction.commit();
}

/** @brief Deletes all the records in the BridgeUserIds database identified by the the logged in user.
 *
 *  It finds the ID of the logged in user. Then it removes all records that include the ID of the logged in user.
 */
void Session::deleteBridgeUserId(){
  dbo::Transaction transaction(session_);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("userID_id = ?").bind(this->user().id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleting BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}
/** @brief Deletes a record in the BridgeUserIds database identified by ip,port and the the logged in user.
 *
 *  It finds the ID of the logged in user and the ID of the bridge by the information passed in by the parameters.
 *  Then it is found and removed in the BridgeUserIds db.
 *
 *  @param ip is used to get the bridge id to be removed from BridgeUserId.
 *  @param port is used to get the bridge id to be removed from BridgeUserId.
 */
void Session::deleteBridgeUserId(std::string ip, std::string port){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());
  Wt::log("info") << "BridgeUserId deleted: "<< y.modify()->bridgeUserID;
  y.remove();  
  transaction.commit();
}

/** @brief Deletes a record in the BridgeUserIds database identified by bridgeObj and the the logged in user.
 *
 *  It finds the ID of the logged in user and the ID of the bridge by the information passed in by the parameters.
 *  Then it is found and removed in the BridgeUserIds db.
 *
 *  @param bridgeObj is used to get the bridge id to be removed from BridgeUserId.
 */
void Session::deleteBridgeUserId(Bridge *bridgeObj){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId deleted: "<< y.modify()->bridgeUserID;
  y.remove();
  transaction.commit();
}

/** @brief Deletes all the records in the BridgeUserIds database.
 *
 *  Removes all records from the BridgeUserIds database.
 *
 *  Iteratively deletes all records in BridgeUserIds.
 */
void Session::deleteAllBridgeUserId(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>();
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;
  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleted BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}

/** @brief Deletes all the records in the BridgeUserIds database that include have ip and port.
 *
 *  It finds the ID of the bridge in the Bridges db and then querys all the records of BridgeUserIds
 *  with the found bridge id. Then iteratively deletes all records in BridgeUserIds.
 *
 *  @param ip of the bridge id to remove all records of bridge id from BridgeUserId.
 *  @param port of the bridge id to remove all records of bridge id from BridgeUserId.
 */
void Session::deleteAllBridgeUserId(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleted BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}
/** @brief Deletes all the records in the BridgeUserIds database that include the bridgeObj.
 *
 *  It finds the ID of the bridge in the Bridges db and then querys all the records of BridgeUserIds
 *  with the found bridge id. Then iteratively deletes all records in BridgeUserIds.
 *
 *  @param bridgeObj is used to get the bridge id to remove all records of bridge id from BridgeUserId.
 */
void Session::deleteAllBridgeUserId(Bridge *bridgeObj){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Removed BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}

/** @brief Getter method for user authorization information.
 *
 *  Used for setting the model of the authorization service.  
 *
 *  @return Returns the user authorization database.
 */
Auth::AbstractUserDatabase& Session::users()
{
  return *users_;
}
/** @brief Getter method for the current sessions authorization service.
 *
 *  Used for configuring the current session's authorization service. For example 
 *  enabling email verification.
 *
 *  @return Returns the session authorization.
 */
const Auth::AuthService& Session::auth()
{
  return myAuthService;
}
/** @brief Getter method for the password authorization service.
 *
 *  Used for configuring the password authorization service. For example 
 *  setting the validation method for the password.
 *
 *  @return Returns the password authorization.
 */
const Auth::AbstractPasswordService& Session::passwordAuth()
{
  return myPasswordService;
}
/** @brief Getter method for the sessions oAuth service
 *
 *  Used to help setup the OAuth services that will be used by the app
 * 
 *  @return Returns the oAuth service that is set up.
 */
const std::vector<const Auth::OAuthService *>& Session::oAuth()
{
  return myOAuthServices;
}
