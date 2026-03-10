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

static InterfaceTable* ft;

// example of implementing a plugin command with async execution.

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

    // just print out the values
    Print("cmdStage2 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    return true;
}

SCBool cmdStage3(World* world, void* inUserData, void* inReplyAddress) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    // just print out the values
    Print("cmdStage3 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    // scsynth will perform completion message after this returns
    return true;
}

SCBool cmdStage4(World* world, void* inUserData, void* inReplyAddress) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    // just print out the values
    Print("cmdStage4 a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    // scsynth will send /done after this returns
    return true;
}

void cmdCleanup(World* world, void* inUserData) {
    // user data is the command.
    MyCmdData* myCmdData = (MyCmdData*)inUserData;

    Print("cmdCleanup a %g  b %g  x %g  y %g  name %s\n", myCmdData->myPlugin->a, myCmdData->myPlugin->b, myCmdData->x,
          myCmdData->y, myCmdData->name);

    RTFree(world, myCmdData->name); // free the string
    RTFree(world, myCmdData); // free command data
    // scsynth will delete the completion message for you.
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
    const char* name = args->gets("(empty)"); // get the string argument
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

*/

//////////////////////////////////////////////////////////////////////////////////////////////////

// example of implementing a plugin with unit commands

struct UnitCmdDemo : public Unit {
    float value;
};

void UnitCmdDemo_next(UnitCmdDemo* unit, int inNumSamples) { OUT0(0) = unit->value; }

void UnitCmdDemo_Ctor(UnitCmdDemo* unit) {
    SETCALC(UnitCmdDemo_next);
    unit->value = 0.f;
    UnitCmdDemo_next(unit, 1);
}

void UnitCmdDemo_setValue(UnitCmdDemo* unit, sc_msg_iter* args) {
    unit->value = args->getf();
    Print("UnitCmdTest: set value to %f\n", unit->value);
}

/*
 * to test the above, send the server these commands:
 *
 *
 * SynthDef(\u_cmd_test, { UnitCmdDemo.kr; }).add;
 * s.sync;
 * u = Synth(\u_cmd_test);
 * // You have to know the index of the UGen within the Synth.
 * // Here we have a SynthDef with a single UGen and without Synth controls,
 * // so the index will be 0.
 * s.sendMsg(\u_cmd, u.nodeID, 0, \setValue, 4.5);
 * s.sendMsg(\u_cmd, u.nodeID, 0, \setValue, -3.0);
 *
 */

PluginLoad(DemoUGens) {
    ft = inTable;

    // define a plugin command - example code
    gMyPlugin.a = 1.2f;
    gMyPlugin.b = 3.4f;
    DefinePlugInCmd("pluginCmdDemo", cmdDemoFunc, (void*)&gMyPlugin);

    // define a unit command - example code
    DefineSimpleUnit(UnitCmdDemo);
    DefineUnitCmd("UnitCmdDemo", "setValue", (UnitCmdFunc)&UnitCmdDemo_setValue);
}
