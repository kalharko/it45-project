#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>

enum skill {
    LSF = 0,
    LPC = 1
} typedef skill_t;

enum speciality {
    JARDINAGE,
    MECANIQUE,
    MENUISERIE,
    ELECTRICITE,
    MUSIQUE
} typedef speciality_t;

struct agent {
    uint64_t id;
    skill_t skill; // LSF = 0, LPC = 1
    speciality_t speciality;
    uint32_t hours;
} typedef agent_t;

struct mission {
    uint64_t id;

    uint32_t day;
    uint16_t start_time; // in minutes
    uint16_t end_time; // in minutes

    skill_t skill;
    speciality_t speciality;
} typedef mission_t;

#endif // DEFS_H
