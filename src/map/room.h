#pragma once

class Spawner;

class Room {
public:
    Room(const vi2d &_pos, const vi2d &_size);

    Room(std::string _name, const vi2d &_size);

    virtual ~Room() = default;

    int id = -1;
    std::string name;

    vi2d pos;
    vi2d size;

    std::vector<Room *> connectedRooms;

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

    bool doorsClosed = false;
    float doorCloseTimer = 0.f;
    bool specialRoom = false;

    std::vector<Spawner *> spawners;
};

class SpecialRoom : public Room {
public:

    SpecialRoom(const vi2d& _pos, const vi2d& _size)
        : Room(_pos, _size) {
        SpecialRoomInit();
    }

    // Constructor for SpecialRoom that initializes the base class using the second constructor
    SpecialRoom(std::string _name, const vi2d& _size)
        : Room(_name, _size) {
        SpecialRoomInit();
    }

    void SpecialRoomInit()
    {
        specialRoom = true;
        doorsClosed = true;
        canHaveSpawners = false;
    }
};


class TestPuzzleRoom : public SpecialRoom {
public:
    TestPuzzleRoom(const vi2d& _pos, const vi2d& _size)
        : SpecialRoom(_pos, _size) {
        SpecialRoomInit();
    }

    // Constructor for SpecialRoom that initializes the base class using the second constructor
    TestPuzzleRoom(std::string _name, const vi2d& _size)
        : SpecialRoom(_name, _size) {
        SpecialRoomInit();
    }





};