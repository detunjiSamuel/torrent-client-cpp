//
// Created by Adetunji on 19/01/2025.
//

#ifndef PEERDISCOVERY_H
#define PEERDISCOVERY_H
#include <vector>
#include "../tcp/tcplib.h"
#include "Peer.h"
#include "../torrent/torrent.h"


namespace peers {

std::vector<Peer> get_pairs(TorrentFile torrentFile);

TcpConnection handshake_pair(Peer peer, std::string &info_hash,
                             bool should_close_connection = true
                             );
void download_piece(Peer peer , TorrentFile torrent);
}

#endif  // PEERDISCOVERY_H
