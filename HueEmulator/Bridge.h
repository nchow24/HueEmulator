/**
* @file Bridge.h
* @author Daniel Le
* @date November 27, 2017
* @brief Object representing a bridge in the database 
* 
* Bridge object in the database that also holds its associated groups and lights. For bridge data, it stores the name, IP address, location, port number, registered status and user ID
**/

#pragma once
#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <vector>

#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>

#include "User.h"

class User;

class Bridge {
private:
	std::string bridgeName;
	std::string location;
	std::string ipAddress;
	std::string hostName;
	std::string userId;
	bool registered;
	int portNumber;

public:
	Bridge() {
		bridgeName = "";
		location= "";
		ipAddress= "";
		hostName= "";
		userId= "";
		registered= false;
		portNumber= 0;
	}
	~Bridge() {}

/**
* @brief Returns the bridge's name
* @return The bridge name
**/
	std::string getBridgeName() {
		return bridgeName;
	}

/**
* @brief Returns the bridge's location
* @return The bridge location
**/
	std::string getLocation() {
		return location;
	}
/**
* @brief Return the bridge's IP address
* @return The bridge IP address
**/
	std::string getIpAddress() {
		return ipAddress;
	}

/**
* @brief Return the bridge's host name
* @return The bridge host name
**/
	std::string getHostName() {
		return hostName;
	}

/**
* @brief Return the bridge's user ID
* @return The bridge's user Id
**/
	std::string getUserId() {
		return userId;
	}

/**
* @brief Return the bridge's registered status
* @return The bridge's registered status
**/
	bool getRegistered() {
		return registered;
	}

/**
* @brief Return the bridge's port number
* @return The bridge's port number
**/
	int getPortNumber() {
		return portNumber;
	}


/**
* @brief Sets the bridges name
* @param newName The bridge's new name
* @return Void.
**/
	void setBridgeName(std::string newName) {
		bridgeName = newName;
	}

/**
* @brief Sets the bridge's location
* @param newLocation The bridge's location
* @return Void.
**/
	void setLocation(std::string newLocation) {
		location = newLocation;
	}

/**
* @brief Sets the bridge's IP address
* @param newIpAddress The bridge's new IP address
* @return Void.
**/
	void setIpAddress(std::string newIpAddress) {
		ipAddress = newIpAddress;
	}

/**
* @brief Sets the bridge's host name
* @param newHostName The bridge's new host name
* @return Void.
**/
	void setHostName(std::string newHostName) {
		hostName = newHostName;
	}

/**
* @brief Sets the bridge's user Id
* @param newUserId The bridge's new user Id
* @return Void.
**/
	void setUserId(std::string newUserId) {
		userId = newUserId;
	}

/**
* @brief Sets the bridge's registered status
* @param newRegistered The bridge's new registered status
* @return Void.
**/
	void setRegistered(bool newRegistered) {
		registered = newRegistered;
	}

/**
* @brief Sets the bridge's port number
* @param newPortNumber The bridge's new port number
* @return Void.
**/
	void setPortNumber(int newPortNumber) {
		portNumber = newPortNumber;
	}


/**
* @brief Sets these class members as attributes for this entity in the database
*
* @param a An address for the entity
*
**/
	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, bridgeName, "bridgeName");
		Wt::Dbo::field(a, location, "location");
		Wt::Dbo::field(a, ipAddress, "ipAddress");
		Wt::Dbo::field(a, hostName, "hostName");
		Wt::Dbo::field(a, userId, "userId");
		Wt::Dbo::field(a, registered, "registered");
		Wt::Dbo::field(a, portNumber, "portNumber");
	}
};
#endif	/* BRIDGE_H */