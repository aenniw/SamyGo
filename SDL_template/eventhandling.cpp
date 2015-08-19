#include "eventhandling.h"

bool EventHandling::HandleEvents() {
    switch (m_events->type) {
        /* close button clicked */
        case SDL_QUIT:
            return false;
            /* handle the keyboard */
        case SDL_KEYDOWN:
            switch (m_events->key.keysym.sym) {
                case SDLK_ESCAPE:
                    return false;
                default:
                    if (m_log.is_open())
                        m_log << "UNHANDLED KEY::" << m_events->key.keysym.sym << std::endl;
                    std::cout << "UNHANDLED KEY::" << m_events->key.keysym.sym << std::endl;

                    break;
            }
            break;
    }
    return true;
}
