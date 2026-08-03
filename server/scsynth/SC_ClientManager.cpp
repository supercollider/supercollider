#include "SC_ClientManager.h"

#include "SC_CoreAudio.h"
#include "SC_FifoMsg.h"
#include "SC_HiddenWorld.h"
#include "SC_World.h"

static void removeClientCallback(FifoMsg* msg) {
    auto* world = msg->mWorld;
    auto* address = static_cast<ReplyAddress*>(msg->mData);

    world->hw->mClientManager->removeClient(*address);

    delete address;
}

void ClientManager::removeClientDefer(World* world, const ReplyAddress& client) {
    FifoMsg msg;
    msg.Set(world, removeClientCallback, nullptr, new ReplyAddress(client));
    AudioDriver(world)->SendMsgFromEngine(msg);
}
