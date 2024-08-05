#include "entity.h"

void entity_update_position(Entity *e, float dt) {
    assert(e && "entity must be not NULL");

    e->position.x += e->velocity.x * dt;
    e->position.y -= e->velocity.y * dt;
}

void entity_apply_acceleration(Entity *e, Vector2 acc, float dt) {
    assert(e && "entity must be not NULL");

    e->velocity = Vector2Add(e->velocity, Vector2Scale(acc, dt));

    Vector2 min = {
        .x = -e->max_velocity,
        .y = -e->max_velocity,
    };
    Vector2 max = {
        .x = e->max_velocity,
        .y = e->max_velocity,
    };
    e->velocity = Vector2Clamp(e->velocity, min, max);
}
