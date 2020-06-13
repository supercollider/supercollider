/************************************************************************
 *
 * Copyright 2019 Christof Ressi <info@christofressi.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#pragma once

namespace SC { namespace Apple { namespace EventLoop {

// Setup the main application. This function must be called in the
// main thread and before any other calls to Cocoa methods.
void setup();
// Run the event loop. This function must be called in the main thread.
// It blocks until the event loop finishes.
void run();
// Ask the event loop to stop and terminate the program.
// This function can be called from any thread.
void quit();

}}} // namespace SC::Apple::EventLoop
