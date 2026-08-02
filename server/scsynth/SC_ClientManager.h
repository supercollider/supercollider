#pragma once
#include <deque>
#include <map>
#include <set>

#include "SC_Types.h"
#include "SC_ReplyImpl.hpp"

typedef std::map<struct ReplyAddress, uint32> ClientIDDict;
typedef std::deque<int> ClientIDs;
typedef std::set<ReplyAddress> Clients;

/**
 * @brief Manages client access of the server such as password check, client id issuing and handling tcp disconnects.
 */
class ClientManager {
    /// maximum number of clients that the server accepts
    uint32 mMaxUsers;

    Clients* mUsers;
    ClientIDs* mAvailableClientIDs;
    ClientIDDict* mClientIDdict;

    /// the password necessary for login.
    std::optional<std::string> mPassword;


public:
    ClientManager(uint32 maxUsers):
        mMaxUsers(maxUsers),
        mUsers(new Clients()),
        mAvailableClientIDs(new ClientIDs()),
        mClientIDdict(new ClientIDDict()) {
        for (int i = 0; i < mMaxUsers; i++) {
            mAvailableClientIDs->push_back(i);
        }
    }

    /// checks a given password. if password has not been set, this will always return false,
    /// so check with has password before.
    bool checkPassword(const std::string& password) const { return mPassword == password; };

    /// returns if server has a password set
    bool hasPassword() const { return mPassword.has_value(); };

    /// set password for server login
    void setPassword(const std::string& password) { mPassword = password; }

    /// returns true iff the server has a free slot for a new client
    bool clientSlotFree() { return 0 < mMaxUsers; }

    /// returns maximum number of allowed users for this server
    uint32 getMaxUsers() { return mMaxUsers; }
};
