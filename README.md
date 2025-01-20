# Particle Physics Simulator (SDL Graphics Version)

A graphical 2D particle physics simulator that demonstrates gravitational interactions between particles with elastic collisions and boundary conditions. Features real-time visualization using SDL2.

## Features

- Graphical visualization using SDL2
- Continuous simulation with smooth animation
- Gravitational interactions between particles using Newton's law of universal gravitation
- Elastic collisions between particles
- Boundary conditions with elastic walls
- Color-coded particles based on type:
  - Yellow: Sun/massive central body
  - Blue: Earth-like planets
  - Red: Mars-like planets
  - Orange: Jupiter-like gas giants
- Particle size scaled based on mass
- 60 FPS display refresh rate

## Controls

Interactive controls available during simulation:
- Space: Pause/Resume simulation
- + (Plus): Zoom in to get a closer view
- - (Minus): Zoom out to see more of the system
- Right Arrow: Speed up simulation
- Left Arrow: Slow down simulation
- R: Reset simulation to initial state
- Left Mouse Click: Add a new planet-sized particle
- Right Mouse Click: Add a new star-sized particle (massive body)
- Mouse Movement: Determines initial velocity of new particles
- Close window to exit simulation

## Physics Implementation

- Gravitational force calculated using F = G * (m1 * m2) / r^2
- Elastic collisions preserve momentum and energy
- Boundary conditions using elastic wall collisions
- Velocity Verlet integration for particle motion

## Building and Running

For detailed building instructions for all operating systems, see [BUILDING.md](BUILDING.md).

Quick start (Linux/macOS):
```bash
# Make build script executable
chmod +x build.sh

# Build and install dependencies
./build.sh

# Run the simulator
./particle_sim_sdl
```

## Tips

- The simulation starts with a solar system configuration
- Watch how the planets orbit around the central star
- Observe how gravity affects the trajectories of bodies
- Notice the elastic collisions when bodies get too close
- The particle size indicates its mass
- Use zoom controls to observe details or view the entire system
- Adjust simulation speed to observe fast or slow motion effects
- Particle trails show the path of each body
- Click and drag to create new particles with initial velocity
- Left click creates smaller bodies, right click creates massive bodies

Created with [**Solver**](https://solverai.com)
