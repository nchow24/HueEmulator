/** @file Session.h
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

#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <string>

#include <Wt/Auth/Login>

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Sqlite3>

#include "User.h"
#include "Bridge.h"
#include "BridgeUserIds.h"

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

typedef Wt::Dbo::collection<Wt::Dbo::ptr<Bridge>> Bridges_Collection;
typedef Wt::Dbo::collection<Wt::Dbo::ptr<BridgeUserIds>> BridgeUserIds_Collection;
typedef Wt::Dbo::ptr<Bridge> BridgePtr;
typedef Wt::Dbo::ptr<User> UserPtr;
typedef Wt::Dbo::ptr<BridgeUserIds> BridgeUserIds_Ptr;



class Session
{
public:
  static void configureAuth();

  Session();
  ~Session();

  Wt::Auth::AbstractUserDatabase& users();
  Wt::Auth::Login& login() { return login_; }


  /*  
   * These methods deal with the currently logged in user
   */
  std::string userName() const;
  std::string firstName();
  std::string lastName();

  //-------------------------
  //---------User DB--------
  void updateUser(User* newUser);
  User* getUser();

  //-------------------------------------
  //---------Bridge With Users DB--------
  //-------------------------------------
  // ADD
  void addBridgeUserId(Bridge *y, std::string bridgeUserId);  //add to relation table
  // GETTERS
  std::vector<BridgeUserIds> getBridgeUserId();                  //get all the bridgeuserids of currentloy logged in user
  BridgeUserIds* getBridgeUserId(std::string ip, std::string port); //get from specific bridge of currently logged in user
  BridgeUserIds* getBridgeUserId(Bridge *bridgeObj);                //get from specific bridge of currently logged in user

  std::vector<BridgeUserIds> getAllBridgeUserId();                                
  std::vector<BridgeUserIds> getAllBridgeUserId(std::string ip, std::string port);
  std::vector<BridgeUserIds> getAllBridgeUserId(Bridge *bridgeObj);
  // UPDATE
  void updateBridgeUserId(std::string ip, std::string port, std::string newBridgeUserId);
  // DELETE
  void deleteBridgeUserId();
  void deleteBridgeUserId(std::string ip, std::string port);
  void deleteBridgeUserId(Bridge *bridgeObj);
  void deleteAllBridgeUserId(); 
  void deleteAllBridgeUserId(std::string ip, std::string port); 
  void deleteAllBridgeUserId(Bridge *bridgeObj); 

  //--------------------------
  //---------Bridge DB--------
  //--------------------------
  std::vector<Bridge> getBridges();     //currently logged in user
  std::vector<Bridge> getAllBridges();  // all user's bridges

  Bridge* getBridge(std::string ip, std::string port);
  void updateBridge(Bridge* oldBridge, Bridge* newBridge);
  bool addBridge(Bridge* newBridge);
  bool deleteBridge(std::string ip, std::string port);

  static const Wt::Auth::AuthService& auth();
  static const Wt::Auth::AbstractPasswordService& passwordAuth();
  static const std::vector<const Wt::Auth::OAuthService *>& oAuth();

  Wt::Dbo::ptr<User> user();
  Wt::Dbo::ptr<User> user(const Wt::Auth::User& authUser);

private:
  Wt::Dbo::backend::Sqlite3 sqlite3_;
  mutable Wt::Dbo::Session session_;
  UserDatabase *users_;
  Wt::Auth::Login login_;

  
};

#endif //SESSION_H_
