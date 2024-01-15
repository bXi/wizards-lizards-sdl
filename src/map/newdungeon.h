#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <utility>
#include <string>

#include <iostream>

#include "utils/vectors.h"

#include "room.h"
#include "tile.h"

class DungeonGen
{
public:
    int width = 8;
    int height = 8;

    int roomMinSize = 1;
    int roomMaxSize = 3;

    int roomSize = 38;
    int wallSize = 1;

    int totalRoomSize = wallSize + roomSize + wallSize;

private:
    class DungeonGraph {
    public:
        void resize(int size);
        std::vector<int> printShortestDistance(int s, int dest, int v);
        bool BFS(int src, int dest, int v, std::vector<int>* pred, std::vector<int>* dist);
        void add_edge(int src, int dest);
        void clear();

    private:
        std::vector<int>* adj;
    };

    

    enum class Wall {
        North,
        South,
        East,
        West
    };
public:
    vi2d getSize();
    std::vector<std::pair<std::pair<int, int>, float>> edges;
    Room* getRoomById(int roomId);
    std::string getMapString();
    void generateLayout();
    std::vector<Room*> getRooms();

private:
    std::vector<Room*> leafs;
    std::vector<Room*> rooms;
    std::vector<Room*> mustHaveRooms;
    std::vector<Room*> optionalRooms;
    std::vector<std::vector<Room*>> grid;

    std::vector<Room*> reached;
    std::vector<Room*> unreached;

    void generateMustHaveRooms();
    
    void findLongestPath();
    bool isPositionValid(const vi2d& position, const vi2d& size);
    void updateRoomExits(Room* room, const vi2d& neighborPos);
    void addRoom(Room* room);
    void generateMst();
    void connectRooms();
    void upgradeRooms();
    void connectTwoRooms(Room* room1, Room* room2);
};
