#include "SC_ClientManager.h"

#include "SC_HiddenWorld.h"
#include "SC_World.h"

void ClientManager::removeClient(World* world, const ReplyAddress& client) {
    world->hw->mClientManager->removeClient(client);
}
