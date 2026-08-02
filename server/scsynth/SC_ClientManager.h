#pragma once
#include <deque>
#include <map>

#include "SC_Types.h"

typedef std::map<struct ReplyAddress, uint32> ClientIDDict;
typedef std::deque<int> ClientIDs;
typedef std::set<ReplyAddress> Clients;

/**
 * @brief Manages client access of the server such as password check, client id issuing and handling tcp disconnects.
 */
class ClientManager {
    // Clients mUsers;
    // ClientIDs mAvailableClientIDs;
    // ClientIDDict mClientIDdict;

    /// the password necessary for login.
    std::optional<std::string> mPassword;


public:
    /// checks a given password. if password has not been set, this will always return false,
    /// so check with has password before.
    bool checkPassword(const std::string& password) const { return mPassword == password; };

    /// returns if server has a password set
    bool hasPassword() const { return mPassword.has_value(); };

    /// set password for server login
    void setPassword(const std::string& password) { mPassword = password; }
};
