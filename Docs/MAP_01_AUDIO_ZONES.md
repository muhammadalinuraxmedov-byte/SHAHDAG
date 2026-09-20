# SHAHDAG — MAP 01 AUDIO ZONES

## Zone plan

### Exterior street
Rain/wind/traffic ambience, broad stereo image, longer distance attenuation.

### Front yard / porch
Exterior ambience remains present; nearby footsteps and door actions gain priority.

### Entry / living room
Shorter room response, stronger occlusion from walls and doors.

### Hallway
Tighter early reflections and clear positional footsteps.

### Kitchen
Harder reflective surfaces; appliance hum and room tone later.

### Garage / utility
Longer, more metallic response with stronger low-frequency tail.

### Upstairs bedrooms
Softer room response and lower environmental density.

### Bathroom
Small hard-surface room with distinct short reverb.

## Gameplay-critical audio
- Footsteps are surface-aware.
- Door interaction has distinct open/close/breach events.
- Weapon reports use indoor/outdoor acoustic variants.
- Occlusion is applied through walls, doors and major cover.
- NPC voice lines use distance and room context.

## Production rule
No generic one-shot ambience should be used as the final implementation. Final audio is a layered system: source + propagation + room response + ambience + gameplay state.
