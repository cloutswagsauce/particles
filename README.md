# Particle Physics Simulator

A simple 2D particle physics simulator that demonstrates gravitational interactions between particles with elastic collisions and boundary conditions.

## Features

- Gravitational interactions between particles using Newton's law of universal gravitation
- Elastic collisions between particles
- Boundary conditions with elastic walls
- Real-time ASCII visualization
- Energy calculation and tracking
- Auto-simulation mode with adjustable speed
- Preset configurations (Solar System and Binary System)

## Controls

1. Add new particle - Create a new particle with custom position, velocity, and mass
2. Run simulation step - Advance the simulation by one step
3. Toggle auto-simulation mode - Run the simulation continuously
4. Adjust simulation speed - Change the number of physics steps per display update
5. Load solar system preset - Create a simple solar system configuration
6. Load binary system preset - Create a binary star system with orbiting particles
7. Exit - Quit the simulator

## Visualization

The simulator uses ASCII characters to represent particles:
- 'O' - Massive bodies (mass > 1e9)
- 'o' - Medium mass bodies (mass > 1e6)
- '*' - Small mass bodies
- '.' - Empty space

## Physics Implementation

- Gravitational force calculated using F = G * (m1 * m2) / r^2
- Elastic collisions preserve momentum and energy
- Boundary conditions using elastic wall collisions
- Velocity Verlet integration for particle motion

## Usage

Compile the program with:
```
gcc particle_sim.c -o particle_sim -lm
```

Run the simulator:
```
./particle_sim
```

## Tips

- Start with preset configurations to see interesting orbital dynamics
- Experiment with different mass ratios and velocities
- Use auto-simulation mode with different speeds to observe long-term behavior
- Add new particles to create your own gravitational systems

Created with [**Solver**](https://solverai.com)
