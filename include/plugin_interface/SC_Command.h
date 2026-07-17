#pragma once

#include "SC_Types.h"

struct Unit;
struct World;
struct sc_msg_iter;

typedef void (*UnitCmdFunc)(struct Unit* unit, struct sc_msg_iter* args);
typedef void (*UnitCmdFuncEx)(struct Unit* unit, struct sc_msg_iter* args, void* replyAddr);
typedef void (*PlugInCmdFunc)(struct World* inWorld, void* inUserData, struct sc_msg_iter* args, void* replyAddr);

typedef SCBool (*AsyncStageFn)(struct World* inWorld, void* cmdData);
typedef SCBool (*AsyncStageFnEx)(struct World* inWorld, void* cmdData, void* replyAddress);
typedef SCBool (*AsyncUnitStageFn)(struct Unit* unit, void* cmdData, void* replyAddress);
typedef void (*AsyncFreeFn)(struct World* inWorld, void* cmdData);
