//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/applications/common/SocketTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/common/VlanTag_m.h"
#include "inet/protocol/ethernet/EthernetSocketTable.h"

namespace inet {

Define_Module(EthernetSocketTable);

void EthernetSocketTable::initialize()
{
}

bool EthernetSocketTable::createSocket(int socketId, MacAddress sourceAddress, MacAddress destAddress, const Protocol *protocol, int vlanId)
{
    auto it = socketIdToSocketMap.find(socketId);
    if (it != socketIdToSocketMap.end())
        return false;

    Socket *socket = new Socket(socketId);
    socket->sourceAddress = sourceAddress;
    socket->destinationAddress = destAddress;
    socket->protocol = protocol;
    socket->vlanId = vlanId;
    socketIdToSocketMap[socketId] = socket;
    return true;
}

bool EthernetSocketTable::deleteSocket(int socketId)
{
    auto it = socketIdToSocketMap.find(socketId);
    if (it == socketIdToSocketMap.end())
        return false;

    delete it->second;
    socketIdToSocketMap.erase(it);
    return true;
}

std::vector<EthernetSocketTable::Socket*> EthernetSocketTable::findSocketsFor(const Packet *packet) const
{
    std::vector<EthernetSocketTable::Socket*> retval;
    auto protocolTag = packet->findTag<PacketProtocolTag>();
    auto protocol = protocolTag ? protocolTag->getProtocol() : nullptr;
    MacAddress src, dest;
    auto macAddressInd = packet->findTag<MacAddressInd>();
    if (macAddressInd) {
        src = macAddressInd->getSrcAddress();
        dest = macAddressInd->getDestAddress();
    }
    auto vlanInd = packet->findTag<VlanInd>();
    auto vlanId = vlanInd ? vlanInd->getVlanId() : -1;

    for (auto s : socketIdToSocketMap) {
        auto socket = s.second;
        if (!socket->sourceAddress.isUnspecified() && !src.isBroadcast() && src != socket->sourceAddress)
            continue;
        if (!socket->destinationAddress.isUnspecified() && !dest.isBroadcast() && dest != socket->destinationAddress)
            continue;
        if (socket->protocol != nullptr && protocol != socket->protocol)
            continue;
        if (socket->vlanId != -1 && vlanId != socket->vlanId)
            continue;
        retval.push_back(socket);
    }
    return retval;
}

} // namespace inet
