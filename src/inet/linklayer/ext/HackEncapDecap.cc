//
// Copyright (C) OpenSim Ltd.
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

#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/ext/HackEncapDecap.h"
#include "inet/networklayer/common/InterfaceEntry.h"

namespace inet {

Define_Module(HackEncapDecap);

void HackEncapDecap::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
        // TODO: parameter
        headerLength = B(14);
}

void HackEncapDecap::handleMessage(cMessage *msg)
{
    auto gateName = msg->getArrivalGate()->getName();
    if (!strcmp(gateName, "upperLayerIn")) {
        send(msg, "lowerLayerOut");
    }
    else if (!strcmp(gateName, "lowerLayerIn")) {
        auto packet = check_and_cast<Packet *>(msg);
        packet->popAtFront(headerLength);
        // TODO: protocol parameter
        packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ipv4);
        send(msg, "upperLayerOut");
    }
    else
        throw cRuntimeError("Unknown message");
}

} // namespace inet
