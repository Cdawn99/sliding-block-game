#include "entity.h"

void entity_update_position(Entity *e, float dt) {
    assert(e && "entity must be not NULL");

    e->position.x += e->velocity.x * dt;
    e->position.y -= e->velocity.y * dt;
}

void entity_apply_acceleration(Entity *e, Vector2 acc, float dt) {
    assert(e && "entity must be not NULL");

    e->velocity.x += acc.x * dt;
    e->velocity.y += acc.y * dt;

    if (e->velocity.x < -e->max_velocity) {
        e->velocity.x = -e->max_velocity;
    }
    if (e->velocity.x > e->max_velocity) {
        e->velocity.x = e->max_velocity;
    }

    if (e->velocity.y < -e->max_velocity) {
        e->velocity.y = -e->max_velocity;
    }
    if (e->velocity.y > e->max_velocity) {
        e->velocity.y = e->max_velocity;
    }
}
