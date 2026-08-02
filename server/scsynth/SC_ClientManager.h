#pragma once
#include <deque>
#include <map>

#include "SC_Types.h"

typedef std::map<struct ReplyAddress, uint32> ClientIDDict;
typedef std::deque<int> ClientIDs;
typedef std::set<ReplyAddress> Clients;

class ClientManager {
    // uint32 mMaxUsers;
    // Clients mUsers;
    // ClientIDs mAvailableClientIDs;
    // ClientIDDict mClientIDdict;

    std::string mPassword;


public:
    bool checkPassword(const std::string& password) const { return mPassword == password; };

    void setPassword(const std::string& password) { mPassword = password; }
};
