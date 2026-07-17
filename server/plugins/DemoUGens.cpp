/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney and others. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "SC_PlugIn.h"

#include <algorithm>

static InterfaceTable* ft;

// example of implementing a plugin command with async execution.
// NOTE: the "pluginCmdDemo" command is used by the TestPluginCommand unit test!
// If you change this code, make sure to update testsuite/classlibrary/TestPluginCommand.sc.

struct MyPluginData // data for the global instance of the plugin
{
    float a, b;
};

struct MyCmdData // data for each command
{
    MyPluginData* myPlugin;
    float x, y;
    char* name;
};

MyPluginData gMyPlugin; // global

SCBool cmdStage2(World* world, void* inUserData, void* inReplyAddress) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    // just for demonstration purposes, let's assume that a string that says "fail" causes the command to fail.
    if (strcmp(myCmdData->name, "fail") != 0) {
        // "success" -> just print out the values
        Print("cmdStage2 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b,
              myCmdData->x, myCmdData->y, myCmdData->name);

        // return 'true' to continue with stage3.
        return true;
    } else {
        // "fail"
        Print("cmdStage2 a %g  b %g  x %g  y %g  failed!\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b,
              myCmdData->x, myCmdData->y);

        // return 'false' to cancel the command. scsynth will not continue with stage3 and instead go jump
        // to the cleanup function. this also means that no completion message will be performed and no
        // /done message will be sent.
        return false;
    }
}

SCBool cmdStage3(World* world, void* inUserData, void* inReplyAddress) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    // just print out the values
    Print("cmdStage3 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    // if you return 'true' scsynth will perform the completion message (if any) and then continue with stage4.
    return true;
}

SCBool cmdStage4(World* world, void* inUserData, void* inReplyAddress) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    // just print out the values
    Print("cmdStage4 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    // return 'true' to send a /done message (if the command name is not NULL).
    // either way, we will continue with the cleanup function.
    return true;
}

void cmdCleanup(World* world, void* inUserData) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    Print("cmdCleanup a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    RTFree(world, myCmdData->name); // free the string
    RTFree(world, myCmdData); // free command data
}

void cmdDemoFunc(World* inWorld, void* inUserData, struct sc_msg_iter* args, void* replyAddr) {
    Print("->cmdDemoFunc %p\n", inUserData);

    // user data is the plug-in's user data.
    MyPluginData* thePlugInData = (MyPluginData*)inUserData;

    // allocate command data, free it in cmdCleanup.
    MyCmdData* myCmdData = (MyCmdData*)RTAlloc(inWorld, sizeof(MyCmdData));
    if (!myCmdData) {
        Print("cmdDemoFunc: memory allocation failed!\n");
        return;
    }
    myCmdData->myPlugin = thePlugInData;

    // ..get data from args..

    // float arguments
    myCmdData->x = args->getf();
    myCmdData->y = args->getf();

    // how to pass a string argument:
    const char* name = args->gets(); // get the string argument
    if (!name)
        name = "";
    size_t nameSize = strlen(name) + 1;
    myCmdData->name = (char*)RTAlloc(inWorld, nameSize); // allocate space, free it in cmdCleanup.
    if (!myCmdData->name) {
        Print("cmdDemoFunc: memory allocation failed!\n");
        return;
    }
    memcpy(myCmdData->name, name, nameSize); // copy the string

    // how to pass an (optional) completion message.
    // NOTE: there is no need to copy the data!
    // DoAsynchronousCommand will internally make a copy as needed.
    size_t msgSize = args->getbsize();
    const void* msgData = args->rdpos + 4;

    DoAsynchronousCommandEx(inWorld, replyAddr, "pluginCmdDemo", myCmdData, cmdStage2, cmdStage3, cmdStage4, cmdCleanup,
                            msgSize, msgData);

    Print("<-cmdDemoFunc\n");
}

/* to test the above, send the server these commands:

(
SynthDef(\sine, {
    Out.ar(0, SinOsc.ar(800,0,0.2) * 0.01)
}).add;

// listen for /done message
OSCdef(\cmd_test, {
    "pluginCmdDemo done!".postln;
}, '/done', nil, nil, [ \pluginCmdDemo ]);
)

s.sendMsg('/cmd', \pluginCmdDemo, 7, 9, \mno, ['/s_new', \sine, 900, 0, 0]);
s.sendMsg('/n_free', 900);
s.sendMsg('/cmd', \pluginCmdDemo, 7, 9, \mno);
s.sendMsg('/cmd', \pluginCmdDemo, 7, 9);
s.sendMsg('/cmd', \pluginCmdDemo, 7);
s.sendMsg('/cmd', \pluginCmdDemo);
// sending a \fail string causes the command to "fail".
// no completion message is performed and no /done message is sent.
s.sendMsg('/cmd', \pluginCmdDemo, 7, 9, \fail, ['/s_new', \sine, 900, 0, 0]);

*/

//////////////////////////////////////////////////////////////////////////////////////////////////

// example of implementing a plugin with unit commands
// NOTE: the "UnitCmdDemo" UGen and its unit commands are used by the TestUnitCommand unit test!
// If you change this code, make sure to update testsuite/classlibrary/TestUnitCommand.sc.

struct UnitCmdDemo : public Unit {
    float value;
    float* data;
    size_t size;
};

void UnitCmdDemo_next(UnitCmdDemo* unit, int inNumSamples) { OUT0(0) = unit->value; }

void UnitCmdDemo_Ctor(UnitCmdDemo* unit) {
    SETCALC(UnitCmdDemo_next);
    unit->value = 0.f;
    unit->data = nullptr;
    unit->size = 0;
    UnitCmdDemo_next(unit, 1);
}

void UnitCmdDemo_setValue(UnitCmdDemo* unit, sc_msg_iter* args) {
    unit->value = args->getf();
    Print("UnitCmdTest: set value to %f\n", unit->value);
}

struct UnitCmdDemoData {
    float* data;
    size_t size;
    float value;
};

SCBool UnitCmdDemo_stage2(Unit* unit, void* rawData, void* replyAddr) {
    auto cmdData = (UnitCmdDemoData*)rawData;

    // just for demonstration purposes, let's assume that a negative value causes the command to fail.
    if (cmdData->value >= 0.f) {
        Print("UnitCmdDemo_stage2 (NRT): allocate %d floats\n", cmdData->size);
        cmdData->data = (float*)malloc(cmdData->size * sizeof(float));
        std::fill_n(cmdData->data, cmdData->size, cmdData->value);
        // return 'true' to continue with stage3.
        return true;
    } else {
        Print("UnitCmdDemo: 'testCommand' failed with %f\n", cmdData->value);
        // return 'false' to cancel the command. scsynth will not continue with stage3 and instead jump
        // to the cleanup function. this also means that no completion message will be performed and no
        // /done message will be sent.
        return false;
    }
}

SCBool UnitCmdDemo_stage3(Unit* unit, void* rawData, void* replyAddr) {
    auto cmdData = (UnitCmdDemoData*)rawData;

    if (unit) {
        // Unit is still alive
        Print("UnitCmdDemo_stage3 (RT): swap data\n");
        auto demoUnit = (UnitCmdDemo*)unit;
        auto oldData = demoUnit->data;
        demoUnit->data = cmdData->data;
        demoUnit->size = cmdData->size;
        cmdData->data = oldData;

        // if you return 'true' scsynth will perform the completion message (if any)
        // and then continue with stage4. Otherwise we jump to the cleanup function.
        return true;
    } else {
        // The owning Synth has been freed concurrently.
        // We still have to continue with stage4 because we need to free our data.
        // The completion message, however, will *not* be performed.
        Print("WARNING: UnitCmdDemo has been freed while 'testCommand' was still running.\n");
        return true;
    }
}

SCBool UnitCmdDemo_stage4(Unit* unit, void* rawData, void* replyAddr) {
    UnitCmdDemoData* cmdData = (UnitCmdDemoData*)rawData;

    Print("UnitCmdDemo_stage4 (NRT): free data\n");
    free(cmdData->data);

    // return 'true' to send a /done message (if the command name is not NULL)
    // either way, we will continue with the cleanup function.
    return true;
}

void UnitCmdDemo_cleanup(World* world, void* rawData) { RTFree(world, rawData); }

void UnitCmdDemo_testCommand(UnitCmdDemo* unit, sc_msg_iter* args, void* replyAddr) {
    // get and verify arguments before allocating the command struct
    int size = args->geti();
    if (size <= 0) {
        Print("UnitCmdDemo: bad size %d for 'testCommand'\n", size);
        return;
    }
    float value = args->getf();
    // how to pass an (optional) completion message.
    // NOTE: there is no need to copy the data!
    // DoAsyncUnitCommand will internally make a copy as needed.
    size_t msgSize = args->getbsize();
    const void* msgData = args->rdpos + 4;

    auto cmdData = (UnitCmdDemoData*)RTAlloc(unit->mWorld, sizeof(UnitCmdDemoData));

    cmdData->data = nullptr;
    cmdData->size = size;
    cmdData->value = value;

    DoAsyncUnitCommand(unit, replyAddr, "testCommand", cmdData, UnitCmdDemo_stage2, UnitCmdDemo_stage3,
                       UnitCmdDemo_stage4, UnitCmdDemo_cleanup, msgSize, msgData);
}

void UnitCmdDemo_Dtor(UnitCmdDemo* unit) {
    if (unit->data) {
        // delete on NRT thread. We cannot call DoAsyncUnitCommand() in the destructor
        // since the owning Graph is already being deleted. Instead we use SendMsgFromRT().
        FifoMsg msg;
        msg.mWorld = unit->mWorld;
        msg.mData = unit->data;
        msg.mPerformFunc = [](FifoMsg* msg) {
            Print("UnitCmdDemo: free data on NRT thread.\n");
            free(msg->mData);
        };
        msg.mFreeFunc = nullptr;
        SendMsgFromRT(unit->mWorld, msg);
    }
}

/* to test the above, send the server these commands:

(
SynthDef(\sine, {
    Out.ar(0, SinOsc.ar(800,0,0.2) * 0.01)
}).add;

SynthDef(\u_cmd_test, { UnitCmdDemo.kr; }).add;

// listen for /done message
OSCdef(\u_cmd_test, {
    "testCommand done!".postln;
}, '/done', nil, nil, [ 'testCommand' ]);
)

u = Synth(\u_cmd_test);

// First test the simple (synchronous) unit command:
// You have to know the index of the UGen within the Synth.
// Here we have a SynthDef with a single UGen and without Synth controls,
// so the index will be 0.
s.sendMsg('/u_cmd', u.nodeID, 0, \setValue, 4.5);
s.sendMsg('/u_cmd', u.nodeID, 0, \setValue, -3.0);

// Now let's also test the asynchronous unit command:
s.sendMsg('/u_cmd', u.nodeID, 0, \testCommand, 48000, 0.5, ['/s_new', \sine, 900, 0, 0]);
s.sendMsg('/n_free', 900);
s.sendMsg('/u_cmd', u.nodeID, 0, \testCommand, 48000, 0.5);
// no completion message is performed and no /done message is sent.
s.sendMsg('/u_cmd', u.nodeID, 0, \testCommand, 48000, -1.0, ['/s_new', \sine, 900, 0, 0]);

// Note how the asynchronous unit command is safely cancelled
// in case the owning Synth is freed concurrently:
(
s.sendMsg('/u_cmd', u.nodeID, 0, \testCommand, 48000, 0.5);
u.free;
)

*/

PluginLoad(DemoUGens) {
    ft = inTable;

    // define a plugin command - example code

    gMyPlugin.a = 1.2f;
    gMyPlugin.b = 3.4f;
    DefinePlugInCmd("pluginCmdDemo", cmdDemoFunc, (void*)&gMyPlugin);

    // define a unit command - example code

    // First define the unit.
    DefineDtorUnit(UnitCmdDemo);
    // Then define a simple (synchronous) unit command.
    DefineUnitCmd("UnitCmdDemo", "setValue", (UnitCmdFunc)&UnitCmdDemo_setValue);
    // Let's also define an extended (asynchronous) unit command.
    // ('testCommand' calls DoAsyncUnitCommand.)
    DefineUnitCmdEx("UnitCmdDemo", "testCommand", (UnitCmdFuncEx)&UnitCmdDemo_testCommand);
}
