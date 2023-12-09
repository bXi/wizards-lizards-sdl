#pragma once


class Spawner;

class Room
    {
    public:
        Room(const vi2d& _pos, const vi2d& _size);
        Room(std::string _name, const vi2d& _size);

        int id = -1;
        std::string name;

        vi2d pos;
        vi2d size;

        std::vector<Room*> connectedRooms;

        bool start = false;
        bool end = false;
        bool onPath = false;

        int northExits = 0;
        int southExits = 0;
        int eastExits = 0;
        int westExits = 0;

        bool canHaveSpawners = false;
        bool visited = false;
        bool finished = false;
        std::vector<Spawner*> spawners;
    };