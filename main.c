#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/raylib.h"
#include "include/raymath.h"

#define SCREEN_WIDTH 1920 / 2
#define SCREEN_HEIGHT 1080 / 2
#define NUM_PARTICLES 200  

int frames_counter;
// Let's start by defining our particule's structure
typedef struct {
    Vector2 pos;
    Vector2 vel;
    float mass;
    float radius;
    Color color;
} particle;

// We can't forget the gravitational impact on the vertical vilocity
void gravityFun(particle particles[], int numParticles) {
    float deltaTime = GetFrameTime();
    for (int i = 0; i < numParticles; i++) {
        particles[i].vel.y += (9.81 * deltaTime) * (1 / particles[i].mass);   
        particles[i].pos.y += particles[i].vel.y * deltaTime;  
    }
}

// Collision management between borders and particules 
void collisionWalls(particle particles[], int numParticles) {
    float e_x = 0.9;  
    float e_y = 0.7;  

    for (int i = 0; i < numParticles; i++) {
        if (particles[i].pos.x - particles[i].radius <= 0) {
            particles[i].pos.x = particles[i].radius;
            particles[i].vel.x = -particles[i].vel.x * e_x;
        }

        if (particles[i].pos.x + particles[i].radius >= SCREEN_WIDTH) {
            particles[i].pos.x = SCREEN_WIDTH - particles[i].radius;
            particles[i].vel.x = -particles[i].vel.x * e_x;
        }

        if (particles[i].pos.y - particles[i].radius <= 0) {
            particles[i].pos.y = particles[i].radius;
            particles[i].vel.y = -particles[i].vel.y * e_y;
        }

        if (particles[i].pos.y + particles[i].radius >= SCREEN_HEIGHT) {
            particles[i].pos.y = SCREEN_HEIGHT - particles[i].radius;
            particles[i].vel.y = -particles[i].vel.y * e_y;
        }
    }
}

// Collision management amoung particules 
void collisionParticules(particle particles[], int numParticles) {
    for (int i = 0; i < numParticles; i++) {
        for (int j = i + 1; j < numParticles; j++) {
            float dx = particles[j].pos.x - particles[i].pos.x;
            float dy = particles[j].pos.y - particles[i].pos.y;
            float distance = sqrtf(dx * dx + dy * dy);
            float minDist = particles[i].radius + particles[j].radius;

            if (distance < minDist && distance > 0) {
                
                float overlap = minDist - distance;
                float totalMass = particles[i].mass + particles[j].mass;
                
                float moveI = (particles[j].mass / totalMass) * overlap;
                float moveJ = (particles[i].mass / totalMass) * overlap;

                float nx = dx / distance;
                float ny = dy / distance;

                particles[i].pos.x -= nx * moveI;
                particles[i].pos.y -= ny * moveI;
                particles[j].pos.x += nx * moveJ;
                particles[j].pos.y += ny * moveJ;

                
                float vx = particles[j].vel.x - particles[i].vel.x;
                float vy = particles[j].vel.y - particles[i].vel.y;
                float dotProduct = vx * nx + vy * ny;

                if (dotProduct > 0) continue; 

                float impulse = (2 * dotProduct) / totalMass;

                particles[i].vel.x += impulse * particles[j].mass * nx;
                particles[i].vel.y += impulse * particles[j].mass * ny;
                particles[j].vel.x -= impulse * particles[i].mass * nx;
                particles[j].vel.y -= impulse * particles[i].mass * ny;
            }
        }
    }
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particle collision simulator");
    SetTargetFPS(60);

    
    particle particles[NUM_PARTICLES];
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].pos = (Vector2){ GetRandomValue(50, SCREEN_WIDTH - 50), GetRandomValue(50, SCREEN_HEIGHT - 50) };
        particles[i].vel = (Vector2){ GetRandomValue(-50, 50) / 10.0f, GetRandomValue(-50, 50) / 10.0f };
        particles[i].mass = GetRandomValue(1, 5);
        particles[i].radius = GetRandomValue(10, 20);
        particles[i].color = (Color){ GetRandomValue(100, 255), GetRandomValue(100, 255), GetRandomValue(100, 255), 255 };
    }

    while (!WindowShouldClose()) {
        
        gravityFun(particles, NUM_PARTICLES);
        collisionWalls(particles, NUM_PARTICLES);
        collisionParticules(particles, NUM_PARTICLES);

        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < NUM_PARTICLES; i++) {
            DrawCircleV(particles[i].pos, particles[i].radius, particles[i].color);
        }

        DrawFPS(10, 10);
        EndDrawing();

        frames_counter++;
    }

    CloseWindow();
    return EXIT_SUCCESS;
}
