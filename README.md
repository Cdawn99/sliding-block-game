# sliding-block-game
A game with a sliding block. Made using [raylib](https://www.raylib.com/)

## Build project

The project is built using CMake.

```
mkdir build && cd build
cmake ..
cmake --build .
```

## Features checklist

```
[X] Base game
[X] Physics-based movement
[X] Collectable coins to increase score
[X] Enemy block that moves around
    Collisions with said enemy decrease the score
[X] Collision at score 0 results in game over (close game)
[X] Start menu
[X] Collision at score 0 results in game over (return to start menu)
[X] Improve enemy movement
[ ] High-score scoreboard
```
