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
}

#endif  // PEERDISCOVERY_H
