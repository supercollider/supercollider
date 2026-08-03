#pragma once
#include <map>

#include "SC_Types.h"
#include "SC_ReplyImpl.hpp"

/**
 * @brief Manages client access of the server such as password check, client id issuing and handling tcp disconnects.
 */
class ClientManager {
    /// maximum number of clients that the server accepts
    uint32 mMaxUsers;

    std::map<ReplyAddress, uint32> mClientDict;

    /// the password necessary for login.
    std::optional<std::string> mPassword;


public:
    ClientManager(uint32 maxUsers): mMaxUsers(maxUsers) {}

    /**
     * @brief checks a given password and returns true if the password is valid.
     * If no password has been set, this will return true for any password.
     */
    bool checkPassword(const std::string& password) const {
        if (mPassword) {
            return mPassword == password;
        }
        return true;
    };

    /// returns true if the server has set a password
    bool hasPassword() const { return mPassword.has_value(); };

    /// set password for server login
    void setPassword(const std::optional<const std::string>& password) { mPassword = password; }

    /// returns true iff the server has a free slot for a new client
    bool clientSlotFree() const { return mClientDict.size() < mMaxUsers; }

    /// returns maximum number of allowed users for this server
    uint32 getMaxUsers() const { return mMaxUsers; }

    const std::map<ReplyAddress, uint32>& getClients() const { return mClientDict; }

    /// returns true iff passed client was present and got removed
    bool removeClient(const ReplyAddress& client) { return mClientDict.erase(client); }

    /// returns a value iff client has an id and has therefore been registered
    std::optional<uint32> getClientID(const ReplyAddress& address) const {
        auto it = mClientDict.find(address);
        if (it == mClientDict.end()) {
            return {};
        }
        return it->second;
    }

    /**
     * @brief registers a client with
     */
    std::optional<uint32> registerClient(const ReplyAddress& address, std::optional<uint32> requestedID = {}) {
        auto id = getNextClientID(requestedID);
        if (!id)
            return {};
        mClientDict.insert(std::make_pair(address, *id));
        return id;
    }

private:
    bool isClientIDTaken(uint32 id) const {
        for (const auto& [addr, clientID] : mClientDict) {
            if (clientID == id)
                return true;
        }
        return false;
    }

    std::optional<uint32> getNextClientID(std::optional<uint32> requestedID) const {
        if (requestedID && *requestedID < mMaxUsers && !isClientIDTaken(*requestedID)) {
            return requestedID;
        }
        for (uint32 i = 0; i < mMaxUsers; i++) {
            if (!isClientIDTaken(i)) {
                return i;
            }
        }
        return {};
    }
};
