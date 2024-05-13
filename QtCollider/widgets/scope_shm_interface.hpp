/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of Qt GUI for SuperCollider.
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

#include "../../common/server_shm.hpp"
#include <QObject>

namespace QtCollider {

class ScopeShm : public QObject {
public:
    ScopeShm(QObject* parent): QObject(parent), client(0) {}

    server_shared_memory_client* client;
    scope_buffer_reader reader;
};

} // namespace QtCollider
