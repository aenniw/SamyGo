#ifndef EVENTHANDLING_H
#define EVENTHANDLING_H

#include <SDL/SDL.h>
#include <iostream>
#include <fstream>
#include <string>

class EventHandling {
private:
    SDL_Event *m_events;
    std::ofstream m_log;
public:
    EventHandling() {
        m_events = new SDL_Event();
    }

    void initLog(std::string name) {
        m_log.open(name.c_str(), std::ios::app | std::ios::out);
    }

    bool HandleEvents();

    SDL_Event *getEvent() const { return m_events; }

    ~EventHandling() {
        if (m_log.is_open())
            m_log.close();
        delete m_events;
    }
};

#endif // EVENTHANDLING_H
