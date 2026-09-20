# SHAHDAG CHANGELOG

## Map 01 — Atmosphere / Art-Direction Foundation
- Added deterministic primitive material differentiation using UE BasicShapeMaterial.
- Added sky-atmosphere foundation.
- Expanded post-process setup for cinematic readability.
- Added additional exterior practical light sources.
- Added driveway/yard dressing proxies.
- Added glass and door-leaf proxies at the main entrance.
- Added explicit art-direction and audio-zone documentation.
- Preserved the single-project / single-ZIP workflow.


## Map 01 — Composition Pass 2
- Added modular street-light mast/arm/luminaire proxies.
- Added utility cabinets, waste bins and hydrant-style street detail.
- Added porch steps, gutter/downspout, HVAC condenser and utility-meter proxies.
- Added garden stepping stones, planter beds, tree proxies and backyard fence posts.
- Added small interior scale props and bedroom/bathroom furnishing proxies.
- Kept all new geometry tagged for later replacement with final Nanite/PBR assets.

## 2026-09-20 — Mobile Runtime Pivot
- Added `Mobile/` Android runtime branch as the primary playable feedback target.
- Added native OpenGL ES renderer, touch FPS controls, mission/loadout flow, and first procedural residential-operation scene.
- Added GitHub Actions APK build workflow.
- Mobile target is designed around a Poco X3 Pro-class performance budget; photorealistic asset work will be introduced incrementally after runtime feedback.
