#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 100
#define G 6.67430e-11  // Gravitational constant
#define DT 0.01        // Time step
#define ZOOM_FACTOR 1.1       // Zoom in/out factor
#define TIME_SCALE_FACTOR 1.2 // Time speed up/slow down factor
#define COLLISION_DISTANCE 5.0
#define ELASTICITY 0.8
#define SCALE_FACTOR 1.0  // Scale factor for visualization

typedef struct {
    double x, y;       // Position
    double vx, vy;     // Velocity
    double mass;       // Mass
    int active;        // Whether particle is active
    SDL_Color color;   // Particle color
} Particle;

// Global simulation parameters
double current_scale = 1.0;
double current_dt = DT;
int paused = 0;

// Initialize SDL and create window/renderer
SDL_Window* init_window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_Window* window = SDL_CreateWindow("Particle Physics Simulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return window;
}

// Initialize a particle with given parameters
void init_particle(Particle* p, double x, double y, double vx, double vy, double mass, SDL_Color color) {
    p->x = x;
    p->y = y;
    p->vx = vx;
    p->vy = vy;
    p->mass = mass;
    p->active = 1;
    p->color = color;
}

// Calculate gravitational force between two particles
void calculate_force(Particle* p1, Particle* p2, double* fx, double* fy) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double r = sqrt(dx*dx + dy*dy);
    
    if (r < 1e-10) {
        *fx = *fy = 0;
        return;
    }
    
    double f = G * p1->mass * p2->mass / (r * r);
    *fx = f * dx/r;
    *fy = f * dy/r;
}

// Handle collision between particles
void handle_collision(Particle* p1, Particle* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dist = sqrt(dx*dx + dy*dy);
    
    if (dist < COLLISION_DISTANCE) {
        double nx = dx / dist;
        double ny = dy / dist;
        double dvx = p2->vx - p1->vx;
        double dvy = p2->vy - p1->vy;
        double vn = dvx * nx + dvy * ny;
        
        if (vn < 0) {
            double j = -(1 + ELASTICITY) * vn;
            j /= 1/p1->mass + 1/p2->mass;
            
            p1->vx -= (j / p1->mass) * nx;
            p1->vy -= (j / p1->mass) * ny;
            p2->vx += (j / p2->mass) * nx;
            p2->vy += (j / p2->mass) * ny;
        }
    }
}

// Update particle position and velocity
void update_particle(Particle* p) {
    // Simple boundary check
    if (fabs(p->x) > WINDOW_WIDTH/2) {
        p->vx = -p->vx * ELASTICITY;
        p->x = (p->x > 0) ? WINDOW_WIDTH/2 : -WINDOW_WIDTH/2;
    }
    if (fabs(p->y) > WINDOW_HEIGHT/2) {
        p->vy = -p->vy * ELASTICITY;
        p->y = (p->y > 0) ? WINDOW_HEIGHT/2 : -WINDOW_HEIGHT/2;
    }
    
    p->x += p->vx * DT;
    p->y += p->vy * DT;
}

// Initialize solar system configuration
void init_solar_system(Particle* particles, int* num_particles) {
    SDL_Color sun_color = {255, 255, 0, 255};    // Yellow
    SDL_Color earth_color = {0, 0, 255, 255};    // Blue
    SDL_Color mars_color = {255, 0, 0, 255};     // Red
    SDL_Color jupiter_color = {255, 140, 0, 255}; // Orange
    
    *num_particles = 0;
    // Sun at center
    init_particle(&particles[(*num_particles)++], 0, 0, 0, 0, 1e11, sun_color);
    // Earth-like
    init_particle(&particles[(*num_particles)++], 200, 0, 0, 8, 1e5, earth_color);
    // Mars-like
    init_particle(&particles[(*num_particles)++], -300, 0, 0, -6, 8e4, mars_color);
    // Jupiter-like
    init_particle(&particles[(*num_particles)++], 400, 0, 0, 5, 5e5, jupiter_color);
}

int main() {
    // Initialize random number generator
    srand(time(NULL));
    
    // Enable relative mouse mode for velocity calculation
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Window* window = init_window();
    if (!window) {
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    Particle particles[MAX_PARTICLES];
    int num_particles = 0;
    
    // Initialize solar system
    init_solar_system(particles, &num_particles);
    
    int running = 1;
    SDL_Event e;
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        paused = !paused;
                        break;
                    case SDLK_EQUALS:  // Plus key
                        current_scale /= ZOOM_FACTOR;  // Zoom in
                        break;
                    case SDLK_MINUS:
                        current_scale *= ZOOM_FACTOR;  // Zoom out
                        break;
                    case SDLK_RIGHT:
                        current_dt *= TIME_SCALE_FACTOR;  // Speed up
                        break;
                    case SDLK_LEFT:
                        current_dt /= TIME_SCALE_FACTOR;  // Slow down
                        break;
                    case SDLK_r:  // Reset simulation
                        init_solar_system(particles, &num_particles);
                        current_scale = 1.0;
                        current_dt = DT;
                        paused = 0;
                        break;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT && num_particles < MAX_PARTICLES) {
                    // Convert screen coordinates to simulation coordinates
                    double sim_x = (e.button.x - WINDOW_WIDTH/2) * SCALE_FACTOR * current_scale;
                    double sim_y = (e.button.y - WINDOW_HEIGHT/2) * SCALE_FACTOR * current_scale;
                    
                    // Get mouse motion for initial velocity
                    int mouse_dx, mouse_dy;
                    SDL_GetRelativeMouseState(&mouse_dx, &mouse_dy);
                    
                    // Create new particle with random color
                    SDL_Color new_color = {
                        rand() % 156 + 100,  // Avoid too dark colors
                        rand() % 156 + 100,
                        rand() % 156 + 100,
                        255
                    };
                    
                    // Mass based on right/left click
                    double mass = (e.button.button == SDL_BUTTON_LEFT) ? 1e5 : 1e9;
                    
                    init_particle(&particles[num_particles],
                                sim_x, sim_y,
                                mouse_dx * 0.1, mouse_dy * 0.1,
                                mass,
                                new_color);
                    num_particles++;
                }
            }
        }
        
        if (paused) {
            SDL_Delay(100);  // Reduce CPU usage while paused
            continue;
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Update particles
        for (int i = 0; i < num_particles; i++) {
            if (!particles[i].active) continue;
            
            double total_fx = 0, total_fy = 0;
            
            // Calculate forces from all other particles
            for (int j = 0; j < num_particles; j++) {
                if (i == j || !particles[j].active) continue;
                
                double fx, fy;
                calculate_force(&particles[i], &particles[j], &fx, &fy);
                total_fx += fx;
                total_fy += fy;
                
                handle_collision(&particles[i], &particles[j]);
            }
            
            // Update particle
            if (!paused) {
                particles[i].x += particles[i].vx * current_dt;
                particles[i].y += particles[i].vy * current_dt;
                update_particle(&particles[i]);
            }
            
            // Draw particle
            SDL_SetRenderDrawColor(renderer, 
                particles[i].color.r,
                particles[i].color.g,
                particles[i].color.b,
                particles[i].color.a);
                
            // Calculate screen coordinates
            int screen_x = (int)(particles[i].x / (SCALE_FACTOR * current_scale) + WINDOW_WIDTH/2);
            int screen_y = (int)(particles[i].y / (SCALE_FACTOR * current_scale) + WINDOW_HEIGHT/2);
            
            // Draw circle using multiple points
            int radius = (int)(sqrt(particles[i].mass) / 1e4) + 2;
            for (int degree = 0; degree < 360; degree++) {
                double radian = degree * M_PI / 180.0;
                int x = screen_x + (int)(cos(radian) * radius);
                int y = screen_y + (int)(sin(radian) * radius);
                SDL_RenderDrawPoint(renderer, x, y);
            }
            
            // Draw particle trail (fading effect)
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (int trail = 1; trail <= 10; trail++) {
                SDL_SetRenderDrawColor(renderer, 
                    particles[i].color.r,
                    particles[i].color.g,
                    particles[i].color.b,
                    255 / (trail * 2));  // Fade out
                
                int trail_x = (int)((particles[i].x - particles[i].vx * DT * trail) / SCALE_FACTOR + WINDOW_WIDTH/2);
                int trail_y = (int)((particles[i].y - particles[i].vy * DT * trail) / SCALE_FACTOR + WINDOW_HEIGHT/2);
                SDL_RenderDrawLine(renderer, screen_x, screen_y, trail_x, trail_y);
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        
        // Present render
        SDL_RenderPresent(renderer);
        
        // Add small delay to control simulation speed
        SDL_Delay(16);  // Approximately 60 FPS
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
