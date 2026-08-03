#pragma once
#include <map>

#include "SC_Types.h"
#include "SC_ReplyImpl.hpp"

struct World;

using ClientID = uint32;

/**
 * @brief Manages client access of the server such as password check, client id issuing and handling tcp disconnects.
 */
class ClientManager {
    /// since this gets accessed in the real time thread, this should only be accessed via
    /// in protected environments such as stage1, stage2, ... or in the real time thread.
    std::map<ReplyAddress, ClientID> mClientDict;

    /// the password necessary for login.
    std::optional<std::string> mPassword {};

    /// maximum number of clients that the server accepts
    uint32 mMaxUsers;

public:
    /**
     * @param maxUsers maximum number of clients that can connect to the server
     */
    ClientManager(uint32 maxUsers): mMaxUsers(maxUsers) {}

    /**
     * @brief checks a given password and returns true if the password is valid.
     * If no password has been set, this will return true for any password.
     */
    bool checkPassword(const std::string& password) const { return mPassword ? password == *mPassword : true; }

    /// returns true if the server has set a password
    bool hasPassword() const { return mPassword.has_value(); };

    /// set password for server login
    void setPassword(const std::optional<const std::string>& password) { mPassword = password; }

    /// returns maximum number of allowed users for this server
    uint32 getMaxUsers() const { return mMaxUsers; }

    /**
     * @brief Returns the dict of all registered clients and their ID.
     * Only access this in (N)RT locked environments!
     */
    const std::map<ReplyAddress, ClientID>& getClients() const { return mClientDict; }

    /**
     * @brief returns true iff passed client was present and got removed
     * Only call this in (N)RT locked environments!
     */
    bool removeClient(const ReplyAddress& client) { return mClientDict.erase(client); }

    /**
     * @brief remove client from list of registered client by defering it via the message queue.
     * This can be called from any thread (though not RT thread!)
     */
    static void removeClientDefer(World* world, const ReplyAddress& client);

    /**
     * @brief returns a value iff client has an id and has therefore been registered
     * Only call this in (N)RT locked environments!
     */
    std::optional<ClientID> getClientID(const ReplyAddress& address) const {
        if (auto it = mClientDict.find(address); it != mClientDict.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    /**
     * @brief registers a client with an optional given ID.
     * Only call this in (N)RT locked environments!
     */
    std::optional<ClientID> registerClient(const ReplyAddress& address, std::optional<ClientID> requestedID = {}) {
        auto id = getNextClientID(requestedID);
        if (!id)
            return {};
        mClientDict.insert(std::make_pair(address, *id));
        return id;
    }

private:
    bool isClientIDTaken(ClientID id) const {
        for (const auto& [addr, clientID] : mClientDict) {
            if (clientID == id)
                return true;
        }
        return false;
    }

    std::optional<ClientID> getNextClientID(std::optional<ClientID> requestedID) const {
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
