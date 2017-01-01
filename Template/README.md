# SamyGo example SDL application

Example Content library application for Samsung LE46B650 (Tested),
  application provides simple showcase of using images ttf and handling of key inputs.
  
Application uses provided libSDL from TV and includes statically linked libSDL_image and libSDL_ttf, that
is necessary. For debugging it is needed to also install on host OS those libraries. 
Key events from remote control are translated to standard/user SDL events.