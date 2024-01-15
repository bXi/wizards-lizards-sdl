#pragma once

enum class Tile : char {
    VOID = '.',
    FLOOR = ' ',
    WALL = '#',
    CLOSEDVDOOR = '+',
    OPENVDOOR = '|',

    CLOSEDHDOOR = '=',
    OPENHDOOR = '-',

    UPSTAIRS = '<',
    DOWNSTAIRS = '>',
};