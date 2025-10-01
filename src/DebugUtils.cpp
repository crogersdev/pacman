#include "DebugUtils.hpp"

std::ostream &operator<<(std::ostream &os, Ghost::State s) {
    switch(s) {
    case Ghost::State::CHASE:
        os << "CHASE";
        break;
    case Ghost::State::FRIGHTENED:
        os << "FRIGHTENED";
        break;
    case Ghost::State::SCATTER:
        os << "SCATTER";
        break;
    case Ghost::State::MEANDER:
        os << "MEANDER";
        break;
    case Ghost::State::IN_PRISON:
        os << "IN_PRISON";
        break;
    case Ghost::State::LEAVING_PRISON:
        os << "LEAVING_PRISON";
        break;
    case Ghost::State::GOING_TO_PRISON:
        os << "GOING_TO_PRISON";
        break;
    default:
        os << "duh";
    }
    return os;
}