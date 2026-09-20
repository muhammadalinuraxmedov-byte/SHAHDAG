# SHAHDAG — MAP 01 ART DIRECTION

## Target
Photorealistic tactical-FPS presentation built around a compact residential operation space.

This milestone does **not** claim final AAA art. It establishes the lighting, material palette, environment scale and atmosphere contracts that final authored assets must respect.

## Material language
- Road / asphalt: dark, high-roughness, subtle variation; wetness will be introduced during the final weather/material pass.
- Sidewalk / concrete: cool neutral concrete with visible aggregate and edge wear in final assets.
- Exterior walls: warm neutral plaster / painted masonry.
- Doors and wood: dark walnut / painted wood with moderate roughness.
- Glass: cool, low-roughness surface with final refraction/reflection treatment.
- Metal: slightly desaturated, rough industrial surfaces except polished fixtures.
- Furniture: muted fabrics and wood; no saturated arcade colors.

## Lighting language
- Exterior: soft directional daylight with atmospheric perspective.
- Interior: practical warm fixtures with cooler spill from windows.
- Garage / utility: colder, harder practical light.
- Avoid flat ambient fill that erases shadow depth.
- Avoid crushed blacks that hide gameplay-critical geometry.
- Lighting must preserve readable silhouettes while retaining the dark tactical mood.

## Geometry language
The final environment is expected to contain:
- believable wall thickness and trim,
- modeled doors and frames,
- realistic window assemblies,
- baseboards and thresholds,
- electrical outlets / switches,
- exposed or concealed cable detail where believable,
- furniture with physically plausible proportions,
- small clutter that supports the room's purpose rather than random decoration.

## Performance contract
Every environment asset must have a defined scalability strategy:
- Nanite where appropriate,
- conventional LODs when Nanite is not appropriate,
- texture resolution tiers,
- shadow contribution budget,
- collision complexity budget,
- streaming priority.

## Validation
The final look must be validated in a real UE5 build. This repository does not claim runtime GPU performance until an actual UE5 build is executed on target hardware.
