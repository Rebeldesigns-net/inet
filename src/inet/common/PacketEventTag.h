//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __INET_PACKETEVENTTAG_H
#define __INET_PACKETEVENTTAG_H

#include "inet/common/PacketEventTag_m.h"
#include "inet/common/TimeTag.h"

namespace inet {

template <typename T>
void insertPacketEvent(const Ptr<Chunk>& chunk, int kind, simtime_t duration, const cModule *module) {
    auto simulation = module->getSimulation();
    auto chunkLength = chunk->getChunkLength();
    chunk->addTagsWhereAbsent<PacketEventTag>(b(0), chunkLength);
    chunk->mapAllTags<PacketEventTag>(b(0), chunkLength, [&] (b offset, b length, PacketEventTag *eventTag) {
        auto packetEvent = new PacketEvent();
        packetEvent->setKind(kind);
        packetEvent->setModulePath(module->getFullPath().c_str());
        packetEvent->setEventNumber(simulation->getEventNumber());
        packetEvent->setSimulationTime(simulation->getSimTime());
        packetEvent->setDuration(duration);
        eventTag->insertPacketEvents(packetEvent);
    });
    increaseTimeTag<T>(chunk, duration);
}

template <typename T>
void insertPacketEvent(Packet *packet, int kind, simtime_t duration, const cModule *module) {
    packet->updateData([&] (const Ptr<Chunk>& chunk) {
        insertPacketEvent<T>(chunk, kind, duration, module);
    });
}

} // namespace inet

#endif // ifndef __INET_PACKETEVENTTAG_H

