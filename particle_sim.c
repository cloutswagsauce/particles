/*
 * Particle Physics Simulator
 * 
 * This program simulates the motion of particles under gravitational forces
 * with collision detection and boundary conditions. Features include:
 * - Gravitational interactions between particles
 * - Elastic collisions
 * - Boundary conditions with elastic walls
 * - Energy calculation
 * - ASCII visualization
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define DISPLAY_WIDTH 40
#define DISPLAY_HEIGHT 20

// Function to map simulation coordinates to display coordinates
int map_to_display(double value, double max_value, int display_size) {
    return (int)((value + max_value) * (display_size - 1) / (2 * max_value));
}

// Display particles in ASCII grid
void display_particles(Particle* particles, int num_particles) {
    char display[DISPLAY_HEIGHT][DISPLAY_WIDTH + 1];
    
    // Initialize display with empty space
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        memset(display[i], '.', DISPLAY_WIDTH);
        display[i][DISPLAY_WIDTH] = '\0';
    }
    
    // Map particles to display
    for (int i = 0; i < num_particles; i++) {
        if (!particles[i].active) continue;
        
        int x = map_to_display(particles[i].x, BOUNDARY_X, DISPLAY_WIDTH);
        int y = map_to_display(particles[i].y, BOUNDARY_Y, DISPLAY_HEIGHT);
        
        // Ensure coordinates are within bounds
        x = x < 0 ? 0 : (x >= DISPLAY_WIDTH ? DISPLAY_WIDTH - 1 : x);
        y = y < 0 ? 0 : (y >= DISPLAY_HEIGHT ? DISPLAY_HEIGHT - 1 : y);
        
        // Use different characters based on particle mass
        char particle_char = (particles[i].mass > 1e9) ? 'O' : 
                           (particles[i].mass > 1e6) ? 'o' : '*';
        display[y][x] = particle_char;
    }
    
    // Print the display
    printf("\033[2J\033[H");  // Clear screen and move cursor to top
    printf("Simulation Display (%.0f x %.0f units)\n", BOUNDARY_X * 2, BOUNDARY_Y * 2);
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        printf("%s\n", display[i]);
    }
    printf("Legend: O = massive body, o = medium mass, * = small mass\n");
}
#define MAX_PARTICLES 100
#define G 6.67430e-11  // Gravitational constant
#define DT 0.01        // Time step
#define COLLISION_DISTANCE 5.0  // Distance at which particles collide
#define ELASTICITY 0.8        // Coefficient of restitution for collisions
#define BOUNDARY_X 500.0      // Boundary limits for x-axis
#define BOUNDARY_Y 500.0      // Boundary limits for y-axis
typedef struct {
    double x, y;       // Position
    double vx, vy;     // Velocity
    double mass;       // Mass
    int active;        // Whether particle is active
} Particle;

// Initialize a particle with given parameters
void init_particle(Particle* p, double x, double y, double vx, double vy, double mass) {
    p->x = x;
    p->y = y;
    p->vx = vx;
    p->vy = vy;
    p->mass = mass;
    p->active = 1;
}

// Calculate gravitational force between two particles
void calculate_force(Particle* p1, Particle* p2, double* fx, double* fy) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double r = sqrt(dx*dx + dy*dy);
    
    if (r < 1e-10) {  // Avoid division by zero
        *fx = *fy = 0;
        return;
    }
    
    double f = G * p1->mass * p2->mass / (r * r);
    *fx = f * dx/r;
    *fy = f * dy/r;
}

// Handle collision between two particles
void handle_collision(Particle* p1, Particle* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double dist = sqrt(dx*dx + dy*dy);
    
    if (dist < COLLISION_DISTANCE) {
        // Normal vector components
        double nx = dx / dist;
        double ny = dy / dist;
        
        // Relative velocity
        double dvx = p2->vx - p1->vx;
        double dvy = p2->vy - p1->vy;
        
        // Normal velocity component
        double vn = dvx * nx + dvy * ny;
        
        // Only handle collision if particles are moving towards each other
        if (vn < 0) {
            // Calculate impulse
            double j = -(1 + ELASTICITY) * vn;
            j /= 1/p1->mass + 1/p2->mass;
            
            // Apply impulse
            p1->vx -= (j / p1->mass) * nx;
            p1->vy -= (j / p1->mass) * ny;
            p2->vx += (j / p2->mass) * nx;
            p2->vy += (j / p2->mass) * ny;
        }
    }
}

// Calculate total energy of a particle
double calculate_energy(Particle* p) {
    // Kinetic energy
    double ke = 0.5 * p->mass * (p->vx * p->vx + p->vy * p->vy);
    return ke;
}

// Handle boundary conditions
void handle_boundary(Particle* p) {
    // X-axis boundaries
    if (p->x > BOUNDARY_X) {
        p->x = BOUNDARY_X;
        p->vx = -p->vx * ELASTICITY;
    } else if (p->x < -BOUNDARY_X) {
        p->x = -BOUNDARY_X;
        p->vx = -p->vx * ELASTICITY;
    }
    
    // Y-axis boundaries
    if (p->y > BOUNDARY_Y) {
        p->y = BOUNDARY_Y;
        p->vy = -p->vy * ELASTICITY;
    } else if (p->y < -BOUNDARY_Y) {
        p->y = -BOUNDARY_Y;
        p->vy = -p->vy * ELASTICITY;
    }
}

// Update particle position and velocity
void update_particle(Particle* p, double fx, double fy) {
    double ax = fx / p->mass;
    double ay = fy / p->mass;
    
    p->vx += ax * DT;
    p->vy += ay * DT;
    p->x += p->vx * DT;
    p->y += p->vy * DT;
    
    handle_boundary(p);
}

// Add a new particle to the simulation
int add_particle(Particle* particles, int* num_particles, double x, double y, double vx, double vy, double mass) {
    if (*num_particles >= MAX_PARTICLES) {
        printf("Error: Maximum number of particles reached\n");
        return 0;
    }
    
    init_particle(&particles[*num_particles], x, y, vx, vy, mass);
    (*num_particles)++;
    return 1;
}

// Display menu and handle user input
// Global simulation parameters
int auto_mode = 0;
int simulation_speed = 1;  // Number of physics steps per display update

// Preset configurations
void load_solar_system(Particle* particles, int* num_particles) {
    *num_particles = 0;
    // Sun (at center)
    add_particle(particles, num_particles, 0, 0, 0, 0, 1e11);
    // Earth-like
    add_particle(particles, num_particles, 200, 0, 0, 8, 1e5);
    // Mars-like
    add_particle(particles, num_particles, -300, 0, 0, -6, 8e4);
    // Jupiter-like
    add_particle(particles, num_particles, 400, 0, 0, 5, 5e5);
}

void load_binary_system(Particle* particles, int* num_particles) {
    *num_particles = 0;
    // Two massive bodies
    add_particle(particles, num_particles, -100, 0, 0, 4, 5e10);
    add_particle(particles, num_particles, 100, 0, 0, -4, 5e10);
    // Orbiting particles
    add_particle(particles, num_particles, 0, 150, 5, 0, 1e4);
    add_particle(particles, num_particles, 0, -150, -5, 0, 1e4);
}

void display_menu() {
    printf("\nParticle Physics Simulator\n");
    printf("1. Add new particle\n");
    printf("2. Run simulation step\n");
    printf("3. Toggle auto-simulation mode\n");
    printf("4. Adjust simulation speed (current: %d)\n", simulation_speed);
    printf("5. Load solar system preset\n");
    printf("6. Load binary system preset\n");
    printf("7. Exit\n");
    printf("Enter choice: ");
}

int main() {
    Particle particles[MAX_PARTICLES];
    int num_particles = 0;
    char choice;
    
    // Initialize with a central mass
    add_particle(particles, &num_particles, 0, 0, 0, 0, 1e10);
    
    while (1) {
        display_menu();
        scanf(" %c", &choice);
        
        switch (choice) {
            case '1': {
                double x, y, vx, vy, mass;
                printf("Enter position (x y): ");
                scanf("%lf %lf", &x, &y);
                printf("Enter velocity (vx vy): ");
                scanf("%lf %lf", &vx, &vy);
                printf("Enter mass: ");
                scanf("%lf", &mass);
                
                if (add_particle(particles, &num_particles, x, y, vx, vy, mass)) {
                    printf("Particle added successfully!\n");
                }
                break;
            }
            case '2': {
                // Handle collisions
                for (int i = 0; i < num_particles; i++) {
                    for (int j = i + 1; j < num_particles; j++) {
                        if (!particles[i].active || !particles[j].active) continue;
                        handle_collision(&particles[i], &particles[j]);
                    }
                }
                
                // Calculate and apply forces
                for (int i = 0; i < num_particles; i++) {
                    if (!particles[i].active) continue;
                    
                    double total_fx = 0, total_fy = 0;
                    
                    for (int j = 0; j < num_particles; j++) {
                        if (i == j || !particles[j].active) continue;
                        
                        double fx, fy;
                        calculate_force(&particles[i], &particles[j], &fx, &fy);
                        total_fx += fx;
                        total_fy += fy;
                    }
                    
                    update_particle(&particles[i], total_fx, total_fy);
                    double energy = calculate_energy(&particles[i]);
                    printf("Particle %d: x=%.2f, y=%.2f, vx=%.2f, vy=%.2f, E=%.2e\n", 
                           i, particles[i].x, particles[i].y, particles[i].vx, particles[i].vy, energy);
                }
                display_particles(particles, num_particles);
                break;
            }
            case '3': {
                auto_mode = !auto_mode;
                printf("Auto-simulation mode %s\n", auto_mode ? "enabled" : "disabled");
                if (auto_mode) {
                    printf("Press Ctrl+C to stop auto-simulation\n");
                    while (auto_mode) {
                        // Run multiple physics steps per display update
                        for (int step = 0; step < simulation_speed; step++) {
                            // Handle collisions
                            for (int i = 0; i < num_particles; i++) {
                                for (int j = i + 1; j < num_particles; j++) {
                                    if (!particles[i].active || !particles[j].active) continue;
                                    handle_collision(&particles[i], &particles[j]);
                                }
                            }
                            
                            // Calculate and apply forces
                            for (int i = 0; i < num_particles; i++) {
                                if (!particles[i].active) continue;
                                
                                double total_fx = 0, total_fy = 0;
                                
                                for (int j = 0; j < num_particles; j++) {
                                    if (i == j || !particles[j].active) continue;
                                    
                                    double fx, fy;
                                    calculate_force(&particles[i], &particles[j], &fx, &fy);
                                    total_fx += fx;
                                    total_fy += fy;
                                }
                                
                                update_particle(&particles[i], total_fx, total_fy);
                            }
                        }
                        
                        // Display updated state
                        for (int i = 0; i < num_particles; i++) {
                            if (!particles[i].active) continue;
                            double energy = calculate_energy(&particles[i]);
                            printf("Particle %d: x=%.2f, y=%.2f, vx=%.2f, vy=%.2f, E=%.2e\n", 
                                   i, particles[i].x, particles[i].y, particles[i].vx, particles[i].vy, energy);
                        }
                        display_particles(particles, num_particles);
                        
                        // Add a small delay to make the animation visible
                        struct timespec delay = {0, 50000000};  // 50ms delay
                        nanosleep(&delay, NULL);
                    }
                }
                break;
            }
            case '4': {
                printf("Enter new simulation speed (steps per display update): ");
                scanf("%d", &simulation_speed);
                if (simulation_speed < 1) simulation_speed = 1;
                printf("Simulation speed set to %d\n", simulation_speed);
                break;
            }
            case '5': {
                load_solar_system(particles, &num_particles);
                printf("Solar system configuration loaded\n");
                break;
            }
            case '6': {
                load_binary_system(particles, &num_particles);
                printf("Binary system configuration loaded\n");
                break;
            }
            case '7':
                printf("Exiting simulator...\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}
