/** @file User.h
*  @class User
*  @brief The model of the user information.
*  @author Paul Li
*  @date Nov 28, 2017
*/
#ifndef USER_H_
#define USER_H_

#include <Wt/WDateTime>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Auth/Dbo/AuthInfo>

#include <string>
#include "Bridge.h"

class User;
class Bridge;

typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;
typedef Wt::Dbo::collection< Wt::Dbo::ptr<User> > Users;

class User
{
public:
  /** @brief User constructor
  *
  *  Empty constructor
  */
  User();

  std::string name; 
  std::string firstName;
  std::string lastName;
  std::string email;
  std::string bridgeUserID;
  std::string customMode;
  
  Wt::Dbo::collection<Wt::Dbo::ptr<Bridge> > bridges;
  Wt::Dbo::collection< Wt::Dbo::ptr<AuthInfo> > authInfos;

  /** @brief Used in dbo to represent the table in the database.
  *
  *  Add fields and the relation types of the user model for the dbo to store and organize.
  *  
  * @param a used by the dbo for creating the database.
  */
  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, name, "name");
    Wt::Dbo::field(a, firstName, "firstName");
    Wt::Dbo::field(a, lastName, "lastName");
    Wt::Dbo::field(a, email, "email");
    Wt::Dbo::field(a, bridgeUserID, "bridgeUserID");
    Wt::Dbo::field(a, customMode, "CustomMode");
    Wt::Dbo::hasMany(a, bridges, Wt::Dbo::ManyToMany, "bridge_user");
    Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
  }
};

DBO_EXTERN_TEMPLATES(User);

#endif // USER_H_
