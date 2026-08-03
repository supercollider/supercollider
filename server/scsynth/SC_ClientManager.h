#pragma once
#include <deque>
#include <map>
#include <set>

#include "SC_Types.h"
#include "SC_ReplyImpl.hpp"

typedef std::map<ReplyAddress, uint32> ClientIDDict;
typedef std::deque<int> ClientIDs;
typedef std::set<ReplyAddress> Clients;

/**
 * @brief Manages client access of the server such as password check, client id issuing and handling tcp disconnects.
 */
class ClientManager {
    /// maximum number of clients that the server accepts
    uint32 mMaxUsers;

    // @todo we actually only need the dictionary, other stuff is redundant
    Clients mUsers;
    ClientIDs mAvailableClientIDs;
    ClientIDDict mClientIDdict;

    /// the password necessary for login.
    std::optional<std::string> mPassword;


public:
    ClientManager(uint32 maxUsers): mMaxUsers(maxUsers) {
        for (uint32 i = 0; i < mMaxUsers; i++) {
            mAvailableClientIDs.push_back(i);
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
    bool clientSlotFree() const { return mClientIDdict.size() < mMaxUsers; }

    /// returns maximum number of allowed users for this server
    uint32 getMaxUsers() { return mMaxUsers; }

    const Clients& getClients() const { return mUsers; }

    void addClient(const ReplyAddress& client) { mUsers.insert(client); }

    /// returns true iff passed client was present and got removed
    bool removeClient(const ReplyAddress& client) {
        auto const it = mUsers.find(client);
        if (it == mUsers.end())
            return false;

        mAvailableClientIDs.push_back(mClientIDdict.at(client));
        mClientIDdict.erase(client);
        mUsers.erase(client);
        return true;
    }

    ClientIDDict& getClientIDDict() { return mClientIDdict; }

    void addClientId(uint32 clientID, ReplyAddress address) { mClientIDdict.insert(std::make_pair(address, clientID)); }

    /** @brief Attempts to find and remove the requested \c id from \c availableIDs.
     *
     * If \c id is -1 or not in \c availableIDs, returns the first element in
     * \c availableIDs. Otherwise, returns \c id.
     */
    int popAvailableClientID(int const id) {
        int clientID = -1;
        if (id == -1) {
            // no requested clientID
            clientID = mAvailableClientIDs.front(); // pop an ID
            mAvailableClientIDs.pop_front();
        } else {
            // user ID requested
            auto it = std::find(mAvailableClientIDs.begin(), mAvailableClientIDs.end(), id);
            if (it != mAvailableClientIDs.end()) { // return the requested ID if available
                clientID = id;
                mAvailableClientIDs.erase(it);
            } else {
                // otherwise return the first free one
                clientID = mAvailableClientIDs.front();
                mAvailableClientIDs.pop_front();
            }
        }

        return clientID;
    }
};
