/*
SuperCollider real time audio synthesis system wasm binding
    Copyright (c) 2026 Dennis Scheiba. All rights reserved.
    https://supercollider.github.io/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <ostream>

#include "PyrLexer.h"
#include "PyrObject.h"
#include "SC_LanguageClient.h"

/** @brief Implements a language client which redirects and exposes
 *  all I/O to a JavaScript interface via emscripten.
 */
class SC_WasmClient : public SC_LanguageClient {
public:
    SC_WasmClient(char const* name): SC_LanguageClient(name) {}

    void postText(const char* str, size_t len) override;

    void postFlush(const char* str, size_t len) override;

    void postError(const char* str, size_t len) override;

    void flush() override;

    void onLibraryStartup() override;

    void runCode(const std::string& code);

    /**
     * (Re-arms) the single AppClock tick timer, replacing the pending one.
     */
    void scheduleTick(double delayMs);
    // id of the pending timeout, 0 if none has been set yet
    int mTickTimeoutId = 0;

    /**
     * Evaluates the code w/o printing to stdout.
     * This becomes necessary for e.g. providing reflections to the editor.
     * @param code
     */
    void runCodeSilent(const std::string& code);

    int run(int argc, char** argv) override;

    /**
     * from SC_TerminalClient.
     * instead of using boost asio, we are using JS callbacks
     */
    void ticker();
};
