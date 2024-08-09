#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include <raymath.h>

#include <assert.h>
#include <math.h>

typedef struct Entity {
    Vector2 position;   /**< The location of the entity's center. */
    Vector2 velocity;
    float max_velocity; /**< The entity's max velocity must be positive. */
    unsigned int score;
    Rectangle sprite;
    Color color;
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
 * given entity. The velocity is clamped according
 * to the entity's max_velocity.
 *
 * @param e The given entity.
 * @param acc The acceleration to be applied.
 * @param dt The time step.
 */
void entity_apply_acceleration(Entity *e, Vector2 acc, float dt);

/**
 * Bounce the given entity off of the walls of the screen.
 *
 * @param e The given entity.
 * @param screen_width The width of the screen.
 * @param screen_height The height of the screen.
 */
void entity_apply_boundary_conditions(Entity *e, int screen_width, int screen_height);

/**
 * Move the entity according to a given instantaneous acceleration.
 * The acceleration is used to update the entity's velocity, which in turn is used to update it's position.
 * The final two parameters are used to apply boundary conditions on the entity according to the following function:
 * @see entity_apply_boundary_conditions
 *
 * @param e The given entity.
 * @param acceleration The instantaneous acceleration that is action on the entity.
 * @param dt The time step within which the acceleration is acting on the entity.
 * @param screen_width The width of the screen.
 * @param screen_height The height of the screen.
 */
void entity_move(Entity *e, Vector2 acceleration, float dt, int screen_width, int screen_height);

/**
 * Check if the entities collided and modify their velocities accordingly (elastic collision).
 *
 * @param e1 Pointer to the first entity.
 * @param e2 Pointer to the second entity.
 * @param error The entities are considered as collided, as long as they are within `error` distance of eachother.
 * @return bool Whether the collision happened.
 */
bool entity_elastic_collision(Entity *e1, Entity *e2);

#endif // ENTITY_H
