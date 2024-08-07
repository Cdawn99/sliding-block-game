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

void entity_apply_boundary_conditions(Entity *e, int screen_width, int screen_height) {
    if ((e->velocity.x > 0 && e->sprite.x + e->sprite.width >= screen_width)
        || (e->velocity.x < 0 && e->sprite.x <= 0)) {
        e->velocity.x = -e->velocity.x;
    }

    if ((e->velocity.y > 0 && e->sprite.y <= 0)
        || (e->velocity.y < 0 && e->sprite.y + e->sprite.height >= screen_height)) {
        e->velocity.y = -e->velocity.y;
    }
}

void entity_move(Entity *e, Vector2 acceleration, float dt, int screen_width, int screen_height) {
    entity_apply_acceleration(e, acceleration, dt);

    e->sprite.x = e->position.x - e->sprite.width/2.0f;
    e->sprite.y = e->position.y - e->sprite.height/2.0f;

    entity_apply_boundary_conditions(e, screen_width, screen_height);

    entity_update_position(e, dt);
}

bool entity_elastic_collision(Entity *e1, Entity *e2) {
    static unsigned int grace = 10;
    bool collided = false;

    if (grace == 0 && CheckCollisionRecs(e1->sprite, e2->sprite)) {
        Vector2 separation = Vector2Subtract(e1->position, e2->position);

        if (fabsf(separation.x) >= fabsf(separation.y)) {
            float temp = e1->velocity.x;
            e1->velocity.x = e2->velocity.x;
            e2->velocity.x = temp;
        }
        if (fabsf(separation.x) <= fabsf(separation.y)) {
            float temp = e1->velocity.y;
            e1->velocity.y = e2->velocity.y;
            e2->velocity.y = temp;
        }

        collided = true;
        grace = 10;
    } else if (grace != 0) {
        grace--;
    }

    return collided;
}
