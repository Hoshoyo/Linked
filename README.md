# Linked

## What is Linked?
Linked is a multiplayer game idealized by Hoshoyo. It is currently being developed by Hoshoyo and Felipeek. We are looking for contributors, specially artists that are able to make sprites and 3d objects. Linked is a non-profit project and is being made for fun.

## Game
![Monster AI](http://puu.sh/kRoVu/7463202ad3.jpg)
![Night](http://puu.sh/kVyID/37992b7699.jpg)
![MultiPlayer](http://puu.sh/kVyA1/de09997992.jpg)

## Build
Linked can be built with Visual Studio 2013 or Visual Studio 2015. Maybe you will have to retarget the solution when building it.

For now, the game can be played Single Player or Multiplayer. However, it is defined at compile-time. You can change it in Game.cpp:

    bool Game::multiplayer = false/true;
    int Game::server_port = 9090;
    std::string Game::server_ip = Server IP (string);

To play multiplayer, the server must be running.

Unfortunately, the lib folder is not in the repository, so Linked cannot be built just by cloning our repository.

## Developer Contact
1. Felipe Kersting (fekersting@inf.ufrgs.br)
2. Pedro Veiga (psveiga@inf.ufrgs.br)
