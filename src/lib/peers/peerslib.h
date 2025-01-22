//
// Created by Adetunji on 19/01/2025.
//

#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H
#include <vector>

#include "Peer.h"

namespace peers {

std::vector<Peer> get_pairs(const std::string &tracker_url,
                                   const std::string &info_hash,
                                   const std::string &length);

void handshake_pair(Peer peer , std::string &info_hash);
}

#endif  // PEERDISCOVERY_H
