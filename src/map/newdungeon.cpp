#include "newdungeon.h"

#include "entities/spawner.h"

void DungeonGen::DungeonGraph::resize(int size)
{
    adj = new std::vector<int>[size];
}

std::vector<int> DungeonGen::DungeonGraph::printShortestDistance(int s, int dest, int v)
{
    const auto pred = new std::vector<int>(v);
    const auto dist = new std::vector<int>(v);

    if (BFS(s, dest, v, pred, dist) == false) {
        return {};
    }

    std::vector<int> path;
    int crawl = dest;
    path.push_back(crawl);
    while (pred->at(crawl) != -1) {
        path.push_back(pred->at(crawl));
        crawl = pred->at(crawl);
    }

    delete pred;
    delete dist;

    return path;
}

bool DungeonGen::DungeonGraph::BFS(int src, int dest, int v, std::vector<int>* pred, std::vector<int>* dist)
{
    std::list<int> queue;

    std::vector<bool> visited(v, false);

    for (int i = 0; i < v; i++) {
        dist->at(i) = INT_MAX;
        pred->at(i) = -1;
    }

    visited[src] = true;
    dist->at(src) = 0;
    queue.push_back(src);

    while (!queue.empty()) {
        int u = queue.front();
        queue.pop_front();

        for (unsigned int i = 0; i < adj[u].size(); i++) {
            if (visited[adj[u][i]] == false) {
                visited[adj[u][i]] = true;
                dist->at(adj[u][i]) = dist->at(u) + 1;
                pred->at(adj[u][i]) = u;
                queue.push_back(adj[u][i]);

                if (adj[u][i] == dest)
                    return true;
            }
        }
    }

    return false;
}

void DungeonGen::DungeonGraph::add_edge(int src, int dest)
{
    adj[src].push_back(dest);
    adj[dest].push_back(src);
}

void DungeonGen::DungeonGraph::clear()
{
    delete[] adj;
}

Room::Room(const vi2d& _pos, const vi2d& _size)
    : pos(_pos), size(_size)
{
}

Room::Room(std::string _name, const vi2d& _size)
    : name(std::move(_name)), size(_size)
{
}

vi2d DungeonGen::getSize()
{
    return { width, height };
}

void DungeonGen::generateMustHaveRooms()
{
    mustHaveRooms.clear();
}

Room* DungeonGen::getRoomById(int roomId)
{
    for (const auto& room : rooms)
        if (room->id == roomId) return room;
    return nullptr;
}

void DungeonGen::generateLayout()
{
    // Clear the existing rooms
    rooms.clear();

    grid.resize(0, std::vector<Room*>(0, nullptr));
    grid.resize(width, std::vector<Room*>(height, nullptr));

    generateMustHaveRooms();

    for (auto& room : mustHaveRooms) {
        rooms.push_back(room);
    }



    // Generate and place additional rooms
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!grid[x][y]) {
                int roomWidth = Helpers::GetRandomValue(roomMinSize, roomMaxSize);
                int roomHeight = Helpers::GetRandomValue(roomMinSize, roomMaxSize);

                if (Helpers::GetRandomValue(0, 100) < 50) {
                    // Higher chance for 1 by 1 rooms
                    roomWidth = roomHeight = 1;
                }
                else {
                    // Random size between 2 and 4
                    roomWidth = Helpers::GetRandomValue(2, roomMaxSize);
                    roomHeight = Helpers::GetRandomValue(2, roomMaxSize);
                }


                bool placed = false;

                while (!placed) {
                    bool canPlace = true;
                    for (int offsetY = 0; offsetY < roomHeight; ++offsetY) {
                        for (int offsetX = 0; offsetX < roomWidth; ++offsetX) {
                            int checkX = x + offsetX;
                            int checkY = y + offsetY;

                            if (checkX >= width || checkY >= height || grid[checkX][checkY]) {
                                canPlace = false;
                                break;
                            }
                        }
                        if (!canPlace)
                            break;
                    }

                    if (canPlace) {
                        Room* newRoom = new Room("Room", { roomWidth, roomHeight });
                        newRoom->pos = { x, y };
                        newRoom->canHaveSpawners = true;
                        addRoom(newRoom);

                        placed = true; // Room placed successfully, exit the retry loop
                    }
                    else {
                        // Retry with a new random room size
                        roomWidth = Helpers::GetRandomValue(roomMinSize, roomMaxSize);
                        roomHeight = Helpers::GetRandomValue(roomMinSize, roomMaxSize);
                    }
                }
            }
        }
    }

    generateMst();
    findLongestPath();
    upgradeRooms();
    connectRooms();

}

void DungeonGen::upgradeRooms() {
    for (auto& room : rooms) {
        if (room->size == vi2d(1,1) && (!room->start && !room->end) && std::rand() % 100 > 50) {

            Room* testRoom = new TestPuzzleRoom("Test puzzle", room->size);
            testRoom->id = room->id;
            testRoom->pos = room->pos;

            room = testRoom;
        }

    }
}

void DungeonGen::findLongestPath() {
    for (const auto &room: rooms) room->connectedRooms.clear();
    leafs.clear();

    auto g = new DungeonGraph();
    g->resize(static_cast<int>(rooms.size()));

    for (auto &edge: edges) {
        const auto firstRoom = getRoomById(edge.first.first);
        const auto secondRoom = getRoomById(edge.first.second);

        firstRoom->connectedRooms.push_back(secondRoom);
        secondRoom->connectedRooms.push_back(firstRoom);

        g->add_edge(firstRoom->id, secondRoom->id);
    }

    for (const auto &room: rooms) {
        if (room->connectedRooms.size() == 1) {
            leafs.push_back(room);
        }
    }

    int maxFoundLength = 0;
    std::vector<int> foundPath;

    for (const auto &roomEnd: leafs) {
        for (const auto &roomStart: leafs) {
            if (roomStart->id != roomEnd->id) {
                auto path = g->printShortestDistance(roomStart->id, roomEnd->id, static_cast<int>(rooms.size()));

                if (static_cast<int>(path.size()) > maxFoundLength) {
                    maxFoundLength = static_cast<int>(path.size());
                    foundPath = path;
                }
            }
        }
    }

    g->clear();
    delete g;

    const int start = Helpers::GetRandomValue(0, 3);
    //const int end = Helpers::GetRandomValue(maxFoundLength - 4, maxFoundLength - 1);
    bool doneStart = false;
    //bool doneEnd = false;

    int j = 0;
    for (const auto &roomId: foundPath) {
        const auto room = getRoomById(roomId);
        if (!doneStart && start == j && room->size == vi2d(1, 1) && !room->specialRoom) {
            room->start = true;
            room->name = "Start";
            room->canHaveSpawners = false;
            doneStart = true;
        }
        if (room->size == vi2d(1, 1) && j >= maxFoundLength - 4 && !room->specialRoom) {
            room->end = true;
            room->name = "End";
            room->canHaveSpawners = false;
            break;
        }
        room->onPath = true;
        j++;
    }

    if (!doneStart) {
        // int j = 0;
        for (int k = 4; k >= 0; k--) {
            for (const auto &roomId: foundPath) {
                const auto room = getRoomById(roomId);
                if (!doneStart && start == k && room->size == vi2d(1, 1) && !room->specialRoom) {
                    room->start = true;
                    room->name = "Start";
                    room->canHaveSpawners = false;
                    doneStart = true;
                }
                //j++;
            }
        }
    }
    foundPath.clear();
}

bool DungeonGen::isPositionValid(const vi2d &position, const vi2d &size) {
    for (int x = position.x - 1; x < position.x + size.x + 1; ++x) {
        for (int y = position.y - 1; y < position.y + size.y + 1; ++y) {
            if (x < 0 || x >= width || y < 0 || y >= height) {
                // The position is outside the dungeon bounds
                return false;
            }

            if (grid[x][y] != nullptr) {
                // There is already a room at the position
                return false;
            }
        }
    }

    return true;
}

void DungeonGen::updateRoomExits(Room *room, const vi2d &neighborPos) {
    if (neighborPos.y < room->pos.y) {
        room->northExits |= (1 << (neighborPos.x - room->pos.x));
    } else if (neighborPos.y > room->pos.y) {
        room->southExits |= (1 << (neighborPos.x - room->pos.x));
    } else if (neighborPos.x < room->pos.x) {
        room->westExits |= (1 << (neighborPos.y - room->pos.y));
    } else if (neighborPos.x > room->pos.x) {
        room->eastExits |= (1 << (neighborPos.y - room->pos.y));
    }
}

void DungeonGen::addRoom(Room *room) {
    room->id = static_cast<int>(rooms.size());

    if (room->canHaveSpawners) {
        auto topLeftSpawner = new Spawner();
        topLeftSpawner->pos = vi2d(room->pos * totalRoomSize) + vi2d(3, 3);
        topLeftSpawner->load();
        room->spawners.push_back(topLeftSpawner);

    }

    // Add the room to the dungeon
    rooms.push_back(room);

    // Add the room to the grid
    for (int x = room->pos.x; x < room->pos.x + room->size.x; ++x) {
        for (int y = room->pos.y; y < room->pos.y + room->size.y; ++y) {
            grid[x][y] = room;
        }
    }
}

void DungeonGen::generateMst() {
    edges.clear();
    if (!rooms.empty()) {
        unreached = rooms;
        reached.clear();

        reached.push_back(unreached.front());
        unreached.erase(unreached.begin());

        while (!unreached.empty()) {
            float record = std::numeric_limits<float>::infinity();
            unsigned int rIndex = 0;
            unsigned int uIndex = 0;
            for (unsigned int i = 0; i < reached.size(); i++) {
                for (unsigned int j = 0; j < unreached.size(); j++) {
                    const auto v1 = reached[i];
                    const auto v2 = unreached[j];

                    // Check if the rooms share a straight wall
                    if (!(v2->pos.x == v1->pos.x + v1->size.x && v2->pos.y >= v1->pos.y && v2->pos.y < v1->pos.y + v1->size.y) &&
                        !(v2->pos.x + v2->size.x == v1->pos.x && v2->pos.y >= v1->pos.y && v2->pos.y < v1->pos.y + v1->size.y) &&
                        !(v2->pos.y == v1->pos.y + v1->size.y && v2->pos.x >= v1->pos.x && v2->pos.x < v1->pos.x + v1->size.x) &&
                        !(v2->pos.y + v2->size.y == v1->pos.y && v2->pos.x >= v1->pos.x && v2->pos.x < v1->pos.x + v1->size.x)) {
                        continue; // Skip if the rooms don't share a straight wall
                    }

                    const float d = v2->pos.distanceTo(v1->pos);
                    if (d < record) {
                        record = d;
                        rIndex = i;
                        uIndex = j;
                    }
                }
            }

            float distance = unreached[uIndex]->pos.distanceTo(reached[rIndex]->pos);

            int lowestId = std::min(reached[rIndex]->id, unreached[uIndex]->id);
            int highestId = std::max(reached[rIndex]->id, unreached[uIndex]->id);

            auto edgeIds = std::pair(lowestId, highestId);
            edges.emplace_back(edgeIds, distance);

            std::sort(edges.begin(), edges.end());

            reached.push_back(unreached[uIndex]);
            unreached.erase(unreached.begin() + uIndex);
        }
    }
}

void DungeonGen::connectRooms() {
    for (const auto &edge: edges) {
        auto firstRoom = getRoomById(edge.first.first);
        auto secondRoom = getRoomById(edge.first.second);

        connectTwoRooms(firstRoom, secondRoom);
    }
}

void DungeonGen::connectTwoRooms(Room *room1, Room *room2) {
    // Determine the shared wall based on the relative positions of the rooms
    Wall sharedWall;

    Room *eastRoom = nullptr;
    Room *westRoom = nullptr;
    Room *northRoom = nullptr;
    Room *southRoom = nullptr;


    if (room2->pos.x == room1->pos.x + room1->size.x && room1->pos.y < room2->pos.y + room2->size.y && room2->pos.y < room1->pos.y + room1->size.y) {
        sharedWall = Wall::East;
        eastRoom = room2;
        westRoom = room1;
    } else if (room1->pos.x == room2->pos.x + room2->size.x && room1->pos.y < room2->pos.y + room2->size.y &&
               room2->pos.y < room1->pos.y + room1->size.y) {
        sharedWall = Wall::West;
        eastRoom = room1;
        westRoom = room2;
    } else if (room2->pos.y == room1->pos.y + room1->size.y && room1->pos.x < room2->pos.x + room2->size.x &&
               room2->pos.x < room1->pos.x + room1->size.x) {
        sharedWall = Wall::South;
        northRoom = room1;
        southRoom = room2;
    } else if (room1->pos.y == room2->pos.y + room2->size.y && room1->pos.x < room2->pos.x + room2->size.x &&
               room2->pos.x < room1->pos.x + room1->size.x) {
        sharedWall = Wall::North;
        northRoom = room2;
        southRoom = room1;
    } else {
        return;
    }


    if (sharedWall == Wall::North || sharedWall == Wall::South) {
        int room1WallLeft, room1WallRight = -1;
        int room2WallLeft, room2WallRight = -1;

        room1WallLeft = room1->pos.x;
        room1WallRight = room1->pos.x + room1->size.x;

        room2WallLeft = room2->pos.x;
        room2WallRight = room2->pos.x + room2->size.x;

        std::vector<int> possibleXpositions;

        for (int pos = std::min(room1WallLeft, room2WallLeft); pos <= std::max(room1WallRight, room2WallRight); pos++) {
            if (pos >= room1WallLeft && pos < room1WallRight && pos >= room2WallLeft && pos < room2WallRight) {
                possibleXpositions.push_back(pos);
            }
        }

        if (!possibleXpositions.empty()) {
            int randomIndex = std::rand() % possibleXpositions.size();
            int randomXPosition = possibleXpositions[randomIndex];

            northRoom->southExits |= (1 << (std::clamp(randomXPosition - northRoom->pos.x, 0, northRoom->size.x)));
            southRoom->northExits |= (1 << (std::clamp(randomXPosition - southRoom->pos.x, 0, southRoom->size.x)));
        }
    } else {
        int room1WallTop, room1WallBottom = -1;
        int room2WallTop, room2WallBottom = -1;

        room1WallTop = room1->pos.y;
        room1WallBottom = room1->pos.y + room1->size.y;

        room2WallTop = room2->pos.y;
        room2WallBottom = room2->pos.y + room2->size.y;

        std::vector<int> possibleYpositions;

        for (int pos = std::min(room1WallTop, room2WallTop); pos < std::max(room1WallBottom, room2WallBottom); pos++) {
            if (pos >= room1WallTop && pos < room1WallBottom && pos >= room2WallTop && pos < room2WallBottom) {
                possibleYpositions.push_back(pos);
            }
        }

        if (!possibleYpositions.empty()) {
            int randomIndex = std::rand() % possibleYpositions.size();
            int randomYPosition = possibleYpositions[randomIndex];


            eastRoom->westExits |= (1 << (std::clamp(randomYPosition - eastRoom->pos.y, 0, eastRoom->size.y)));
            westRoom->eastExits |= (1 << (std::clamp(randomYPosition - westRoom->pos.y, 0, westRoom->size.y)));
        }
    }
}

std::string DungeonGen::getMapString() {
    vi2d size = getSize();
    std::string mapString;

    auto drawRectangle = [&](int _height, int _width, int xPos, int yPos, Tile tile, std::string &mapString) {
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                int mapX = xPos + x;
                int mapY = yPos + y;
                int index = mapY * (size.x * totalRoomSize) + mapX;
                mapString.at(index) = static_cast<char>(tile);
            }
        }
    };

    for (int y = 0; y < size.y * totalRoomSize + 2; y++) {
        for (int x = 0; x < size.x * totalRoomSize + 2; x++) {
            mapString += '.';
        }
    }

    for (auto &room: rooms) {
        int _width = room->size.x * totalRoomSize;
        int _height = room->size.y * totalRoomSize;

        int xPos = room->pos.x * totalRoomSize;
        int yPos = room->pos.y * totalRoomSize;

        drawRectangle(_height, _width, xPos, yPos, Tile::WALL, mapString);
        drawRectangle(_height - 2, _width - 2, xPos + 1, yPos + 1, Tile::FLOOR, mapString);

        int doorSize = 4; // Adjust the size of the doorways

        if (room->northExits > 0) {
            int doorX = xPos + (totalRoomSize / 2) - (doorSize / 2);
            int doorY = yPos;
            int doorBit = 1;

            for (int i = 0; i < room->size.x; i++) {
                if (room->northExits & doorBit) {
                    drawRectangle(1, doorSize, doorX, doorY, Tile::FLOOR, mapString);
                }
                doorX += totalRoomSize;
                doorBit <<= 1;
            }
        }

        if (room->southExits > 0) {
            int doorX = xPos + (totalRoomSize / 2) - (doorSize / 2);
            int doorY = yPos + (room->size.y * totalRoomSize) - 1;
            int doorBit = 1;

            for (int i = 0; i < room->size.x; i++) {
                if (room->southExits & doorBit) {
                    drawRectangle(1, doorSize, doorX, doorY, Tile::FLOOR, mapString);
                }
                doorX += totalRoomSize;
                doorBit <<= 1;
            }
        }

        if (room->eastExits > 0) {
            int doorX = xPos + (room->size.x * totalRoomSize) - 1;
            int doorY = yPos + (totalRoomSize / 2) - (doorSize / 2);
            int doorBit = 1;

            for (int i = 0; i < room->size.y; i++) {
                if (room->eastExits & doorBit) {
                    drawRectangle(doorSize, 1, doorX, doorY, Tile::FLOOR, mapString);
                }
                doorY += totalRoomSize;
                doorBit <<= 1;
            }
        }

        if (room->westExits > 0) {
            int doorX = xPos;
            int doorY = yPos + (totalRoomSize / 2) - (doorSize / 2);
            int doorBit = 1;

            for (int i = 0; i < room->size.y; i++) {
                if (room->westExits & doorBit) {
                    drawRectangle(doorSize, 1, doorX, doorY, Tile::FLOOR, mapString);
                }
                doorY += totalRoomSize;
                doorBit <<= 1;
            }
        }

    }



    return mapString;
}

std::vector<Room*> DungeonGen::getRooms() {
    return rooms;

}
