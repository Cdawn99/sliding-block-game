#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <assert.h>
#include <math.h>

typedef struct Entity {
    Vector2 position;
    Vector2 velocity;
    float max_velocity; /**< The entity's max velocity must be positive. */
    unsigned int score;
} Entity;

/**
 * Update the position of the given entity
 * based on its current velocity.
 *
 * @param e The given entity.
 * @param dt The time step.
 */
void entity_update_position(Entity *e, float dt);

/**
 * Apply an instantaneous acceleration to the
 * given entity.
 *
 * @param e The given entity.
 * @param acc The acceleration to be applied.
 * @param dt The time step.
 */
void entity_apply_acceleration(Entity *e, Vector2 acc, float dt);

#endif // ENTITY_H
