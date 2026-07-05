/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
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

#include "SC_LanguageClient.h"
#include <iostream>

int main(int argc, char** argv) {
    SC_LanguageClient* client = createLanguageClient("sclang");
    if (!client)
        return 99;

    // If a return code was set by the sclang user, this will return that, even if the shutdown process throws.
    // If no return code was set, and this throws, it will throw into this scope.
    // If no return code was and no throwing occured then return 0;
    const int returnCode = client->run(argc, argv);

    // Ignore all errors when destroying the language client.
    // We want to preserve the return code the sclang user has given us.
    try {
        destroyLanguageClient(client);
    } catch (...) {
        // We are terminating the program anyway so just ignore everything.
        std::cerr << "An exception was thrown while called destroyLanguageClient\n";
    }
    return returnCode;
}
