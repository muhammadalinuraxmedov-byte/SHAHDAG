#include "ShahdagMapGenerator.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyAtmosphere.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UObjectGlobals.h"

AShahdagMapGenerator::AShahdagMapGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AShahdagMapGenerator::BeginPlay()
{
    Super::BeginPlay();
    if (bBuildOnBeginPlay)
    {
        BuildEnvironment();
    }
}


void AShahdagMapGenerator::ApplyPrimitiveLook(UStaticMeshComponent* Mesh, const FString& Label)
{
    if (!Mesh) return;

    static UMaterialInterface* BaseMaterial = nullptr;
    if (!BaseMaterial)
    {
        BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    }

    if (!BaseMaterial) return;

    FLinearColor Color(0.16f, 0.16f, 0.16f, 1.0f);
    float Roughness = 0.78f;

    if (Label.Contains(TEXT("Road")) || Label.Contains(TEXT("Curb")))
    {
        Color = FLinearColor(0.055f, 0.06f, 0.065f, 1.0f);
        Roughness = 0.92f;
    }
    else if (Label.Contains(TEXT("Sidewalk")) || Label.Contains(TEXT("Path")) || Label.Contains(TEXT("Driveway")))
    {
        Color = FLinearColor(0.24f, 0.25f, 0.25f, 1.0f);
        Roughness = 0.88f;
    }
    else if (Label.Contains(TEXT("Fence")))
    {
        Color = FLinearColor(0.12f, 0.15f, 0.13f, 1.0f);
        Roughness = 0.82f;
    }
    else if (Label.Contains(TEXT("Wall")) || Label.Contains(TEXT("Slab")) || Label.Contains(TEXT("Roof")))
    {
        Color = FLinearColor(0.57f, 0.55f, 0.51f, 1.0f);
        Roughness = 0.83f;
    }
    else if (Label.Contains(TEXT("Glass")) || Label.Contains(TEXT("Window")))
    {
        Color = FLinearColor(0.12f, 0.20f, 0.26f, 1.0f);
        Roughness = 0.18f;
    }
    else if (Label.Contains(TEXT("Door")))
    {
        Color = FLinearColor(0.16f, 0.095f, 0.055f, 1.0f);
        Roughness = 0.58f;
    }
    else if (Label.Contains(TEXT("Stair")) || Label.Contains(TEXT("Rail")))
    {
        Color = FLinearColor(0.38f, 0.34f, 0.30f, 1.0f);
        Roughness = 0.70f;
    }
    else if (Label.Contains(TEXT("Kitchen")) || Label.Contains(TEXT("Cabinet")))
    {
        Color = FLinearColor(0.26f, 0.28f, 0.30f, 1.0f);
        Roughness = 0.66f;
    }
    else if (Label.Contains(TEXT("Bed")) || Label.Contains(TEXT("Sofa")) || Label.Contains(TEXT("Chair")))
    {
        Color = FLinearColor(0.20f, 0.19f, 0.18f, 1.0f);
        Roughness = 0.94f;
    }
    else if (Label.Contains(TEXT("Table")) || Label.Contains(TEXT("Console")) || Label.Contains(TEXT("Nightstand")))
    {
        Color = FLinearColor(0.22f, 0.13f, 0.07f, 1.0f);
        Roughness = 0.60f;
    }
    else if (Label.Contains(TEXT("Vehicle")))
    {
        Color = FLinearColor(0.09f, 0.10f, 0.11f, 1.0f);
        Roughness = 0.34f;
    }
    else if (Label.Contains(TEXT("Dumpster")) || Label.Contains(TEXT("Utility")))
    {
        Color = FLinearColor(0.11f, 0.15f, 0.14f, 1.0f);
        Roughness = 0.74f;
    }

    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, Mesh);
    if (!MID) return;

    MID->SetVectorParameterValue(TEXT("Color"), Color);
    MID->SetScalarParameterValue(TEXT("Roughness"), Roughness);
    Mesh->SetMaterial(0, MID);
}

AActor* AShahdagMapGenerator::CreateBox(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    static UStaticMesh* CubeMesh = nullptr;
    if (!CubeMesh)
    {
        CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    }
    if (!CubeMesh) return nullptr;

    AStaticMeshActor* Box = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(Rotation, LocationCm, FVector(1.0f)));
    if (!Box) return nullptr;

    UStaticMeshComponent* Mesh = Box->GetStaticMeshComponent();
    Mesh->SetStaticMesh(CubeMesh);
    Mesh->SetMobility(EComponentMobility::Static);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->CanCharacterStepUpOn = ECB_Yes;
    Mesh->SetCanEverAffectNavigation(true);
    ApplyPrimitiveLook(Mesh, Label);

    // Keep the primitive foundation simple and deterministic. Final Nanite/PBR assets
    // will replace these actors during the environment-art pass.
    Box->SetActorScale3D(SizeCm / 100.0f);
    Box->Tags.Add(FName(*Label));
    return Box;
}


AActor* AShahdagMapGenerator::CreateCylinder(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    static UStaticMesh* CylinderMesh = nullptr;
    if (!CylinderMesh)
    {
        CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    }
    if (!CylinderMesh) return nullptr;

    AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(Rotation, LocationCm, FVector(1.0f)));
    if (!Actor) return nullptr;

    UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent();
    Mesh->SetStaticMesh(CylinderMesh);
    Mesh->SetMobility(EComponentMobility::Static);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCanEverAffectNavigation(true);
    ApplyPrimitiveLook(Mesh, Label);
    Actor->SetActorScale3D(SizeCm / 100.0f);
    Actor->Tags.Add(FName(*Label));
    return Actor;
}

AActor* AShahdagMapGenerator::CreateSphere(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    static UStaticMesh* SphereMesh = nullptr;
    if (!SphereMesh)
    {
        SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    }
    if (!SphereMesh) return nullptr;

    AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform(Rotation, LocationCm, FVector(1.0f)));
    if (!Actor) return nullptr;

    UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent();
    Mesh->SetStaticMesh(SphereMesh);
    Mesh->SetMobility(EComponentMobility::Static);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCanEverAffectNavigation(true);
    ApplyPrimitiveLook(Mesh, Label);
    Actor->SetActorScale3D(SizeCm / 100.0f);
    Actor->Tags.Add(FName(*Label));
    return Actor;
}

AActor* AShahdagMapGenerator::CreatePointLight(const FVector& LocationCm, float Intensity, const FLinearColor& Color, float Radius)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    APointLight* LightActor = World->SpawnActor<APointLight>(APointLight::StaticClass(), FTransform(FRotator::ZeroRotator, LocationCm));
    if (!LightActor) return nullptr;

    UPointLightComponent* Light = LightActor->GetPointLightComponent();
    Light->SetIntensity(Intensity);
    Light->SetLightColor(Color);
    Light->SetAttenuationRadius(Radius);
    Light->SetMobility(EComponentMobility::Movable);
    return LightActor;
}

AActor* AShahdagMapGenerator::CreateSpotLight(const FVector& LocationCm, const FRotator& Rotation, float Intensity, const FLinearColor& Color, float Radius, float InnerCone, float OuterCone)
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    ASpotLight* LightActor = World->SpawnActor<ASpotLight>(ASpotLight::StaticClass(), FTransform(Rotation, LocationCm));
    if (!LightActor) return nullptr;

    USpotLightComponent* Light = LightActor->GetSpotLightComponent();
    Light->SetIntensity(Intensity);
    Light->SetLightColor(Color);
    Light->SetAttenuationRadius(Radius);
    Light->SetInnerConeAngle(InnerCone);
    Light->SetOuterConeAngle(OuterCone);
    Light->SetMobility(EComponentMobility::Movable);
    return LightActor;
}

void AShahdagMapGenerator::CreateWallSegment(const FVector& A, const FVector& B, float Height, float Thickness, float ZCenter, const FString& Label)
{
    const FVector Mid = (A + B) * 0.5f;
    const FVector Delta = B - A;
    const float Length = Delta.Size();
    const float Yaw = FMath::RadiansToDegrees(FMath::Atan2(Delta.Y, Delta.X));
    CreateBox(FVector(Mid.X, Mid.Y, ZCenter), FVector(Length, Thickness, Height), FRotator(0.0f, Yaw, 0.0f), Label);
}

void AShahdagMapGenerator::CreateDoorFrame(const FVector& Center, float Width, float Height, float WallThickness, const FString& Label)
{
    const float LegH = Height;
    const float SideW = 8.0f;
    const float HeaderH = 8.0f;
    CreateBox(Center + FVector(-(Width * 0.5f - SideW * 0.5f), 0.0f, LegH * 0.5f), FVector(SideW, WallThickness, LegH), FRotator::ZeroRotator, Label + TEXT("_Left"));
    CreateBox(Center + FVector((Width * 0.5f - SideW * 0.5f), 0.0f, LegH * 0.5f), FVector(SideW, WallThickness, LegH), FRotator::ZeroRotator, Label + TEXT("_Right"));
    CreateBox(Center + FVector(0.0f, 0.0f, LegH - HeaderH * 0.5f), FVector(Width, WallThickness, HeaderH), FRotator::ZeroRotator, Label + TEXT("_Header"));
}

void AShahdagMapGenerator::CreateWindowFrame(const FVector& Center, float Width, float Height, float WallThickness, const FString& Label)
{
    const float Frame = 6.0f;
    CreateBox(Center + FVector(-(Width * 0.5f - Frame * 0.5f), 0.0f, Height * 0.5f), FVector(Frame, WallThickness, Height), FRotator::ZeroRotator, Label + TEXT("_Left"));
    CreateBox(Center + FVector((Width * 0.5f - Frame * 0.5f), 0.0f, Height * 0.5f), FVector(Frame, WallThickness, Height), FRotator::ZeroRotator, Label + TEXT("_Right"));
    CreateBox(Center + FVector(0.0f, 0.0f, Height - Frame * 0.5f), FVector(Width, WallThickness, Frame), FRotator::ZeroRotator, Label + TEXT("_Top"));
    CreateBox(Center + FVector(0.0f, 0.0f, Frame * 0.5f), FVector(Width, WallThickness, Frame), FRotator::ZeroRotator, Label + TEXT("_Bottom"));
}

void AShahdagMapGenerator::CreateTable(const FVector& Center, const FVector& Size, const FString& Label)
{
    const FVector TopSize(Size.X, Size.Y, 8.0f);
    CreateBox(Center + FVector(0, 0, Size.Z - 60.0f), TopSize, FRotator::ZeroRotator, Label + TEXT("_Top"));

    const float LegX = Size.X * 0.5f - 25.0f;
    const float LegY = Size.Y * 0.5f - 25.0f;
    const float LegH = Size.Z - 64.0f;
    for (float SX : {-1.0f, 1.0f})
    {
        for (float SY : {-1.0f, 1.0f})
        {
            CreateBox(Center + FVector(SX * LegX, SY * LegY, LegH * 0.5f), FVector(12.0f, 12.0f, LegH), FRotator::ZeroRotator, Label + TEXT("_Leg"));
        }
    }
}

void AShahdagMapGenerator::CreateChair(const FVector& Center, const FRotator& Rotation, const FString& Label)
{
    const FVector LocalSeat(0, 0, 45);
    const FVector LocalBack(0, 22, 105);
    const float LegH = 45.0f;

    auto LocalToWorld = [&Rotation, &Center](const FVector& Local)
    {
        return Center + Rotation.RotateVector(Local);
    };

    CreateBox(LocalToWorld(LocalSeat), FVector(55, 55, 8), Rotation, Label + TEXT("_Seat"));
    CreateBox(LocalToWorld(LocalBack), FVector(55, 8, 105), Rotation, Label + TEXT("_Back"));
    for (float SX : {-18.0f, 18.0f})
    {
        for (float SY : {-18.0f, 18.0f})
        {
            CreateBox(LocalToWorld(FVector(SX, SY, LegH * 0.5f)), FVector(7, 7, LegH), Rotation, Label + TEXT("_Leg"));
        }
    }
}

void AShahdagMapGenerator::CreateBed(const FVector& Center, const FVector& Size, const FRotator& Rotation, const FString& Label)
{
    const float BaseH = 28.0f;
    CreateBox(Center + Rotation.RotateVector(FVector(0, 0, BaseH * 0.5f)), FVector(Size.X, Size.Y, BaseH), Rotation, Label + TEXT("_Base"));
    CreateBox(Center + Rotation.RotateVector(FVector(0, 0, BaseH + 12.0f)), FVector(Size.X - 10.0f, Size.Y - 10.0f, 24.0f), Rotation, Label + TEXT("_Mattress"));
    CreateBox(Center + Rotation.RotateVector(FVector((-Size.X * 0.5f) + 70.0f, 0, BaseH + 34.0f)), FVector(70, Size.Y - 40.0f, 12.0f), Rotation, Label + TEXT("_Pillow"));
    CreateBox(Center + Rotation.RotateVector(FVector((Size.X * 0.5f) - 45.0f, 0, 110.0f)), FVector(10, Size.Y + 20.0f, 120.0f), Rotation, Label + TEXT("_Headboard"));
}

void AShahdagMapGenerator::CreateCabinetRun(const FVector& Start, int32 Count, float Spacing, const FString& LabelPrefix)
{
    for (int32 i = 0; i < Count; ++i)
    {
        const FVector P = Start + FVector(i * Spacing, 0.0f, 42.0f);
        CreateBox(P, FVector(70.0f, 60.0f, 84.0f), FRotator::ZeroRotator, FString::Printf(TEXT("%s_%02d"), *LabelPrefix, i + 1));
    }
}

void AShahdagMapGenerator::BuildEnvironment()
{
    BuildStreetAndBoundaries();
    BuildResidentialHouse();
    BuildExteriorContext();
    BuildStreetFurnitureAndUtilities();
    BuildResidentialExteriorDetails();
    BuildInteriorRooms();
    BuildFurnitureAndProps();
    BuildInteriorDetailPass();
    BuildLighting();
    BuildInteriorLights();
    BuildAtmosphereAndArtDirection();
}


void AShahdagMapGenerator::BuildAtmosphereAndArtDirection()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // A real sky-atmosphere actor gives the scene a coherent horizon and aerial perspective.
    ASkyAtmosphere* Atmosphere = World->SpawnActor<ASkyAtmosphere>(
        ASkyAtmosphere::StaticClass(),
        FTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, 0.0f)));

    if (Atmosphere)
    {
        Atmosphere->SetActorHiddenInGame(false);
    }

    // Subtle nightfall-ready contrast: keep the scene readable indoors while
    // preserving darker exterior pockets for tactical lighting.
    APostProcessVolume* Post = World->SpawnActor<APostProcessVolume>(
        APostProcessVolume::StaticClass(),
        FTransform(FRotator::ZeroRotator));

    if (Post)
    {
        Post->bUnbound = true;
        Post->Priority = 20.0f;
        Post->Settings.AutoExposureMinBrightness = 0.55f;
        Post->Settings.AutoExposureMaxBrightness = 1.15f;
        Post->Settings.BloomIntensity = 0.08f;
        Post->Settings.VignetteIntensity = 0.10f;
        Post->Settings.MotionBlurAmount = 0.0f;
        Post->Settings.LensFlareIntensity = 0.0f;
    }

    // Exterior practicals establish believable light sources before final fixture meshes.
    for (int32 i = -3; i <= 3; ++i)
    {
        const float X = -2600.0f + static_cast<float>(i) * 850.0f;
        CreatePointLight(
            FVector(X, 2900.0f, 430.0f),
            950.0f,
            FLinearColor(0.95f, 0.82f, 0.64f),
            650.0f);
    }
}

void AShahdagMapGenerator::BuildStreetAndBoundaries()
{
    // Compact operational footprint: approximately 140m x 80m.
    CreateBox(FVector(0, 0, -12), FVector(14000, 8000, 24), FRotator::ZeroRotator, TEXT("Road_Base"));
    CreateBox(FVector(0, -3500, 28), FVector(14000, 900, 56), FRotator::ZeroRotator, TEXT("South_Sidewalk"));
    CreateBox(FVector(0, 3500, 28), FVector(14000, 900, 56), FRotator::ZeroRotator, TEXT("North_Sidewalk"));

    // Curb strips define a street without making an invisible play boundary.
    CreateBox(FVector(0, -3050, 55), FVector(14000, 100, 110), FRotator::ZeroRotator, TEXT("South_Curb"));
    CreateBox(FVector(0, 3050, 55), FVector(14000, 100, 110), FRotator::ZeroRotator, TEXT("North_Curb"));

    // Natural perimeter: fences, parked-car masses and closed side roads.
    CreateBox(FVector(-6400, 0, 90), FVector(120, 6100, 180), FRotator::ZeroRotator, TEXT("West_Fence_Perimeter"));
    CreateBox(FVector(6400, 0, 90), FVector(120, 6100, 180), FRotator::ZeroRotator, TEXT("East_Fence_Perimeter"));
    CreateBox(FVector(0, 3850, 90), FVector(12500, 120, 180), FRotator::ZeroRotator, TEXT("North_Perimeter_Fence"));
    CreateBox(FVector(0, -3850, 90), FVector(12500, 120, 180), FRotator::ZeroRotator, TEXT("South_Perimeter_Fence"));

    // Road blockers / environmental limits rather than invisible walls.
    for (int32 i = 0; i < 4; ++i)
    {
        CreateBox(FVector(-5950.0f + i * 420.0f, 0, 70), FVector(280, 200, 140), FRotator(0, 0, 90), FString::Printf(TEXT("Road_Blocker_%02d"), i + 1));
    }
}

void AShahdagMapGenerator::BuildResidentialHouse()
{
    // Two-storey home, approximately 16m x 12m. Coordinate origin is near the foyer.
    const float GroundBottom = 0.0f;
    const float FloorH = 290.0f;
    const float UpperFloorZ = 320.0f;
    const float UpperFloorH = 290.0f;
    const float HalfX = 800.0f;
    const float HalfY = 600.0f;
    const float WallT = 22.0f;

    // Ground floor slab and upper-floor deck.
    CreateBox(FVector(-100, 0, 0), FVector(1640, 1240, 30), FRotator::ZeroRotator, TEXT("Ground_Slab"));
    CreateBox(FVector(-100, 0, UpperFloorZ), FVector(1640, 1240, 70), FRotator::ZeroRotator, TEXT("Upper_Slab"));

    // South facade with a central front-door opening and two windows.
    CreateWallSegment(FVector(-900, -600), FVector(-560, -600), FloorH, WallT, GroundBottom + FloorH * 0.5f, TEXT("Ground_South_Wall_A"));
    CreateWallSegment(FVector(-420, -600), FVector(120, -600), FloorH, WallT, GroundBottom + FloorH * 0.5f, TEXT("Ground_South_Wall_B"));
    CreateWallSegment(FVector(260, -600), FVector(700, -600), FloorH, WallT, GroundBottom + FloorH * 0.5f, TEXT("Ground_South_Wall_C"));
    CreateDoorFrame(FVector(-210, -600, 0), 140.0f, 235.0f, WallT + 8.0f, TEXT("Front_Door_Frame"));
    CreateBox(FVector(-210, -586, 117.5f), FVector(128, 10, 225), FRotator::ZeroRotator, TEXT("Front_Door_Leaf")); 
    CreateWindowFrame(FVector(-675, -600, 115), 160.0f, 140.0f, WallT + 8.0f, TEXT("Front_Window_A"));
    CreateBox(FVector(-675, -588, 115), FVector(140, 8, 120), FRotator::ZeroRotator, TEXT("Front_Window_A_Glass"));
    CreateWindowFrame(FVector(500, -600, 115), 160.0f, 140.0f, WallT + 8.0f, TEXT("Front_Window_B"));
    CreateBox(FVector(500, -588, 115), FVector(140, 8, 120), FRotator::ZeroRotator, TEXT("Front_Window_B_Glass"));

    // North/rear wall with back-door opening.
    CreateWallSegment(FVector(-900, 600), FVector(-350, 600), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_North_Wall_A"));
    CreateWallSegment(FVector(-210, 600), FVector(700, 600), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_North_Wall_B"));
    CreateDoorFrame(FVector(-280, 600, 0), 140.0f, 235.0f, WallT + 8.0f, TEXT("Back_Door_Frame"));

    // Side walls with window bays.
    CreateWallSegment(FVector(-900, -600), FVector(-900, -250), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_West_Wall_A"));
    CreateWallSegment(FVector(-900, -90), FVector(-900, 180), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_West_Wall_B"));
    CreateWallSegment(FVector(-900, 340), FVector(-900, 600), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_West_Wall_C"));
    CreateWindowFrame(FVector(-900, 80, 115), 145.0f, 140.0f, WallT + 8.0f, TEXT("West_Window_A"));

    CreateWallSegment(FVector(700, -600), FVector(700, -200), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_East_Wall_A"));
    CreateWallSegment(FVector(700, -40), FVector(700, 250), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_East_Wall_B"));
    CreateWallSegment(FVector(700, 410), FVector(700, 600), FloorH, WallT, FloorH * 0.5f, TEXT("Ground_East_Wall_C"));
    CreateWindowFrame(FVector(700, 325, 115), 145.0f, 140.0f, WallT + 8.0f, TEXT("East_Window_A"));

    // Upper level facade and two windows per major side.
    const float UZCenter = UpperFloorZ + UpperFloorH * 0.5f;
    CreateWallSegment(FVector(-900, -600), FVector(-620, -600), UpperFloorH, WallT, UZCenter, TEXT("Upper_South_Wall_A"));
    CreateWallSegment(FVector(-420, -600), FVector(40, -600), UpperFloorH, WallT, UZCenter, TEXT("Upper_South_Wall_B"));
    CreateWallSegment(FVector(240, -600), FVector(700, -600), UpperFloorH, WallT, UZCenter, TEXT("Upper_South_Wall_C"));
    CreateWindowFrame(FVector(-520, -600, UpperFloorZ + 150), 180.0f, 150.0f, WallT + 8.0f, TEXT("Upper_Front_Window_A"));
    CreateWindowFrame(FVector(360, -600, UpperFloorZ + 150), 180.0f, 150.0f, WallT + 8.0f, TEXT("Upper_Front_Window_B"));

    CreateWallSegment(FVector(-900, 600), FVector(700, 600), UpperFloorH, WallT, UZCenter, TEXT("Upper_North_Wall"));
    CreateWallSegment(FVector(-900, -600), FVector(-900, 600), UpperFloorH, WallT, UZCenter, TEXT("Upper_West_Wall"));
    CreateWallSegment(FVector(700, -600), FVector(700, 600), UpperFloorH, WallT, UZCenter, TEXT("Upper_East_Wall"));

    // Ground-floor internal planning walls: foyer, kitchen, utility, living room.
    CreateWallSegment(FVector(-100, -580), FVector(-100, -160), FloorH, 18.0f, FloorH * 0.5f, TEXT("Ground_Hall_Wall"));
    CreateWallSegment(FVector(-100, -20), FVector(-100, 280), FloorH, 18.0f, FloorH * 0.5f, TEXT("Ground_Hall_Wall_B"));
    CreateDoorFrame(FVector(-100, -100, 0), 130.0f, 225.0f, 24.0f, TEXT("Hall_Door_Frame"));

    CreateWallSegment(FVector(-820, 220), FVector(-480, 220), FloorH, 18.0f, FloorH * 0.5f, TEXT("Kitchen_Wall_A"));
    CreateWallSegment(FVector(-310, 220), FVector(220, 220), FloorH, 18.0f, FloorH * 0.5f, TEXT("Kitchen_Wall_B"));
    CreateDoorFrame(FVector(-390, 220, 0), 120.0f, 225.0f, 24.0f, TEXT("Kitchen_Door_Frame"));

    CreateWallSegment(FVector(400, 50), FVector(400, 250), FloorH, 18.0f, FloorH * 0.5f, TEXT("Utility_Wall_A"));
    CreateWallSegment(FVector(400, 390), FVector(400, 560), FloorH, 18.0f, FloorH * 0.5f, TEXT("Utility_Wall_B"));
    CreateDoorFrame(FVector(400, 320, 0), 120.0f, 225.0f, 24.0f, TEXT("Utility_Door_Frame"));

    // Stair core: landing plus simple steps toward upper level.
    for (int32 i = 0; i < 11; ++i)
    {
        const float StepW = 260.0f;
        const float StepD = 92.0f;
        const float StepH = 24.0f;
        const float X = 270.0f + i * 42.0f;
        const float Z = 45.0f + i * (UpperFloorZ / 11.0f);
        CreateBox(FVector(X, 425.0f, Z), FVector(StepW, StepD, StepH + i * 2.0f), FRotator(0, 0, 0), FString::Printf(TEXT("Stair_%02d"), i + 1));
    }

    // Roof masses: simple low-pitch silhouette at foundation stage.
    CreateBox(FVector(-100, 0, 760), FVector(1740, 1340, 70), FRotator(0, 0, 0), TEXT("Roof_Main"));
    CreateBox(FVector(-100, -650, 810), FVector(1760, 100, 80), FRotator(0, 0, 0), TEXT("Roof_Front_Edge"));
    CreateBox(FVector(-100, 650, 810), FVector(1760, 100, 80), FRotator(0, 0, 0), TEXT("Roof_Rear_Edge"));

    // Garage / side utility building: intentionally separate to create a tactical exterior route.
    CreateBox(FVector(1120, 160, 220), FVector(620, 900, 440), FRotator::ZeroRotator, TEXT("Garage_Block"));
    CreateDoorFrame(FVector(1120, -290, 0), 220.0f, 235.0f, 30.0f, TEXT("Garage_Main_Door"));
    CreateBox(FVector(1120, 1100, 220), FVector(620, 240, 440), FRotator::ZeroRotator, TEXT("Rear_Storage_Block"));
}

void AShahdagMapGenerator::BuildExteriorContext()
{
    // Driveway and front path.
    CreateBox(FVector(-220, -1200, 22), FVector(520, 700, 44), FRotator::ZeroRotator, TEXT("Driveway"));
    CreateBox(FVector(-220, -760, 30), FVector(180, 420, 60), FRotator::ZeroRotator, TEXT("Front_Path"));

    // Low fence panels around front and side yards.
    CreateBox(FVector(-1220, -920, 35), FVector(220, 140, 70), FRotator::ZeroRotator, TEXT("Planter_A"));
    CreateBox(FVector(1020, -880, 35), FVector(260, 140, 70), FRotator::ZeroRotator, TEXT("Planter_B"));
    CreateBox(FVector(-1120, -1210, 20), FVector(90, 90, 40), FRotator::ZeroRotator, TEXT("Mailbox"));
    CreateBox(FVector(-540, -1110, 16), FVector(75, 75, 32), FRotator::ZeroRotator, TEXT("Path_Light_A"));
    CreateBox(FVector(120, -1110, 16), FVector(75, 75, 32), FRotator::ZeroRotator, TEXT("Path_Light_B"));

    CreateBox(FVector(-100, -1470, 85), FVector(2700, 55, 170), FRotator::ZeroRotator, TEXT("Front_Fence"));
    CreateBox(FVector(-1500, -540, 85), FVector(55, 1900, 170), FRotator::ZeroRotator, TEXT("West_Fence"));
    CreateBox(FVector(1500, -540, 85), FVector(55, 1900, 170), FRotator::ZeroRotator, TEXT("East_Fence"));

    // Dumpster and utility cabinets in rear service area.
    CreateBox(FVector(950, 1320, 95), FVector(150, 240, 190), FRotator::ZeroRotator, TEXT("Dumpster"));
    CreateBox(FVector(600, 1260, 110), FVector(120, 80, 220), FRotator::ZeroRotator, TEXT("Utility_Cabinet"));

    // Two crude vehicle volumes used only to establish believable street scale and cover locations.
    CreateBox(FVector(-260, -1880, 75), FVector(450, 175, 150), FRotator(0, 0, 90), TEXT("Vehicle_A"));
    CreateBox(FVector(460, -1870, 75), FVector(470, 180, 150), FRotator(0, 0, 90), TEXT("Vehicle_B"));
}

void AShahdagMapGenerator::BuildInteriorRooms()
{
    // Ceiling planes are deliberately shallow; future production geometry will replace these with modular ceiling pieces.
    CreateBox(FVector(-100, 0, 300), FVector(1600, 1200, 25), FRotator::ZeroRotator, TEXT("Ground_Ceiling_Proxy"));
    CreateBox(FVector(-100, 0, 625), FVector(1600, 1200, 25), FRotator::ZeroRotator, TEXT("Upper_Ceiling_Proxy"));

    // Upper floor internal partitions: three bedrooms + stair landing.
    CreateWallSegment(FVector(-500, -570), FVector(-500, 140), 260.0f, 18.0f, UpperFloorZ + 130.0f, TEXT("Upper_Bedroom_Wall_A"));
    CreateDoorFrame(FVector(-500, -210, UpperFloorZ), 120.0f, 220.0f, 24.0f, TEXT("Upper_Bedroom_Door_A"));

    CreateWallSegment(FVector(40, -570), FVector(40, 120), 260.0f, 18.0f, UpperFloorZ + 130.0f, TEXT("Upper_Bedroom_Wall_B"));
    CreateDoorFrame(FVector(40, -210, UpperFloorZ), 120.0f, 220.0f, 24.0f, TEXT("Upper_Bedroom_Door_B"));

    CreateWallSegment(FVector(260, 160), FVector(260, 560), 260.0f, 18.0f, UpperFloorZ + 130.0f, TEXT("Upper_Bathroom_Wall_A"));
    CreateDoorFrame(FVector(260, 100, UpperFloorZ), 120.0f, 220.0f, 24.0f, TEXT("Upper_Bathroom_Door"));

    // Small hallway rail/balustrade blocks around stair void.
    CreateBox(FVector(220, 220, UpperFloorZ + 55), FVector(500, 18, 110), FRotator::ZeroRotator, TEXT("Upper_Rail_A"));
    CreateBox(FVector(-20, 450, UpperFloorZ + 55), FVector(18, 300, 110), FRotator::ZeroRotator, TEXT("Upper_Rail_B"));
}

void AShahdagMapGenerator::BuildFurnitureAndProps()
{
    // Ground floor: living room.
    CreateBox(FVector(420, -250, 45), FVector(360, 80, 90), FRotator::ZeroRotator, TEXT("TV_Console"));
    CreateBox(FVector(420, -300, 105), FVector(300, 12, 140), FRotator::ZeroRotator, TEXT("TV_Screen_Proxy"));
    CreateBox(FVector(300, -60, 75), FVector(500, 170, 150), FRotator::ZeroRotator, TEXT("Sofa"));
    CreateTable(FVector(-40, -110, 65), FVector(160, 120, 130), TEXT("Living_Coffee_Table"));

    // Dining zone.
    CreateTable(FVector(-430, -380, 110), FVector(280, 160, 220), TEXT("Dining_Table"));
    CreateChair(FVector(-550, -380, 0), FRotator(0, 180, 0), TEXT("Dining_Chair_A"));
    CreateChair(FVector(-310, -380, 0), FRotator::ZeroRotator, TEXT("Dining_Chair_B"));
    CreateChair(FVector(-430, -500, 0), FRotator(0, 90, 0), TEXT("Dining_Chair_C"));

    // Kitchen counter line and island.
    CreateCabinetRun(FVector(-780, 350, 0), 7, 85.0f, TEXT("Kitchen_Cabinet"));
    CreateBox(FVector(-500, 150, 105), FVector(420, 130, 210), FRotator::ZeroRotator, TEXT("Kitchen_Island"));
    CreateBox(FVector(-500, 155, 215), FVector(420, 140, 10), FRotator::ZeroRotator, TEXT("Kitchen_Countertop"));
    CreateBox(FVector(-770, 350, 125), FVector(120, 8, 60), FRotator::ZeroRotator, TEXT("Kitchen_Sink_Proxy"));
    CreateBox(FVector(-650, 300, 95), FVector(70, 30, 150), FRotator::ZeroRotator, TEXT("Kitchen_Appliance_A"));
    CreateBox(FVector(-540, 300, 95), FVector(70, 30, 150), FRotator::ZeroRotator, TEXT("Kitchen_Appliance_B"));

    // Utility / entry area.
    CreateBox(FVector(460, 350, 100), FVector(160, 80, 200), FRotator::ZeroRotator, TEXT("Utility_Cabinet_A"));
    CreateBox(FVector(580, 350, 100), FVector(80, 80, 200), FRotator::ZeroRotator, TEXT("Utility_Cabinet_B"));
    CreateBox(FVector(-700, -480, 55), FVector(90, 90, 110), FRotator::ZeroRotator, TEXT("Entry_Table"));

    // Upper bedrooms.
    CreateBed(FVector(-690, -220, UpperFloorZ + 35), FVector(190, 360, 90), FRotator::ZeroRotator, TEXT("Bedroom_A_Bed"));
    CreateBed(FVector(-180, -220, UpperFloorZ + 35), FVector(190, 360, 90), FRotator::ZeroRotator, TEXT("Bedroom_B_Bed"));
    CreateBed(FVector(430, 370, UpperFloorZ + 35), FVector(170, 340, 90), FRotator(0, 90, 0), TEXT("Bedroom_C_Bed"));

    CreateBox(FVector(-650, 120, UpperFloorZ + 60), FVector(120, 55, 120), FRotator::ZeroRotator, TEXT("Bedroom_A_Nightstand"));
    CreateBox(FVector(-140, 120, UpperFloorZ + 60), FVector(120, 55, 120), FRotator::ZeroRotator, TEXT("Bedroom_B_Nightstand"));
    CreateBox(FVector(620, 430, UpperFloorZ + 60), FVector(120, 55, 120), FRotator(0, 90, 0), TEXT("Bedroom_C_Nightstand"));

    // Bathroom proxies.
    CreateBox(FVector(520, 470, UpperFloorZ + 45), FVector(130, 90, 90), FRotator::ZeroRotator, TEXT("Bathroom_Vanity"));
    CreateBox(FVector(330, 430, UpperFloorZ + 45), FVector(70, 140, 90), FRotator::ZeroRotator, TEXT("Bathroom_Toilet_Proxy"));
}


void AShahdagMapGenerator::BuildStreetFurnitureAndUtilities()
{
    // Streetlights: simple mast + arm + lamp volume. These are intentionally modular proxies.
    for (int32 i = -4; i <= 4; ++i)
    {
        const float X = i * 1300.0f;
        CreateCylinder(FVector(X, -3060.0f, 285.0f), FVector(9.0f, 9.0f, 520.0f), FRotator::ZeroRotator, TEXT("StreetLamp_Mast"));
        CreateBox(FVector(X + 75.0f, -3060.0f, 535.0f), FVector(150.0f, 12.0f, 12.0f), FRotator::ZeroRotator, TEXT("StreetLamp_Arm"));
        CreateSphere(FVector(X + 145.0f, -3060.0f, 515.0f), FVector(26.0f, 26.0f, 18.0f), FRotator::ZeroRotator, TEXT("StreetLamp_Luminaire"));
    }

    // Utility / telecom cabinet cluster near the north sidewalk.
    for (int32 i = 0; i < 3; ++i)
    {
        CreateBox(FVector(-3600.0f + i * 210.0f, 3000.0f, 95.0f), FVector(120.0f, 90.0f, 190.0f), FRotator::ZeroRotator, TEXT("Utility_Cabinet_Street"));
    }

    // Fire-hydrant-style street detail (visual proxy only).
    CreateCylinder(FVector(1850.0f, -2990.0f, 32.0f), FVector(14.0f, 14.0f, 64.0f), FRotator::ZeroRotator, TEXT("Hydrant_Base"));
    CreateSphere(FVector(1850.0f, -2990.0f, 70.0f), FVector(24.0f, 24.0f, 20.0f), FRotator::ZeroRotator, TEXT("Hydrant_Cap"));

    // Recycling/waste containers tucked into a service corner.
    CreateBox(FVector(-2750.0f, 2500.0f, 90.0f), FVector(170.0f, 150.0f, 180.0f), FRotator::ZeroRotator, TEXT("Waste_Bin_A"));
    CreateBox(FVector(-2550.0f, 2500.0f, 90.0f), FVector(170.0f, 150.0f, 180.0f), FRotator::ZeroRotator, TEXT("Waste_Bin_B"));
}

void AShahdagMapGenerator::BuildResidentialExteriorDetails()
{
    // Front steps / porch geometry.
    for (int32 i = 0; i < 3; ++i)
    {
        CreateBox(FVector(-210.0f, -760.0f - i * 45.0f, 18.0f + i * 16.0f), FVector(210.0f, 70.0f, 36.0f + i * 8.0f), FRotator::ZeroRotator, TEXT("Front_Step"));
    }
    CreateBox(FVector(-210.0f, -640.0f, 250.0f), FVector(250.0f, 35.0f, 18.0f), FRotator::ZeroRotator, TEXT("Porch_Weather_Header"));

    // Gutter/downspout proxies make the facade feel constructed rather than block-generated.
    CreateCylinder(FVector(-885.0f, -620.0f, 405.0f), FVector(5.0f, 5.0f, 820.0f), FRotator::ZeroRotator, TEXT("Downspout_West"));
    CreateCylinder(FVector(690.0f, -620.0f, 405.0f), FVector(5.0f, 5.0f, 820.0f), FRotator::ZeroRotator, TEXT("Downspout_East"));
    CreateBox(FVector(-100.0f, -610.0f, 774.0f), FVector(1720.0f, 24.0f, 26.0f), FRotator::ZeroRotator, TEXT("Front_Gutter"));

    // Exterior HVAC condenser and meter boxes.
    CreateBox(FVector(860.0f, 460.0f, 120.0f), FVector(190.0f, 150.0f, 240.0f), FRotator::ZeroRotator, TEXT("HVAC_Condenser"));
    CreateCylinder(FVector(820.0f, 470.0f, 220.0f), FVector(95.0f, 95.0f, 20.0f), FRotator(90.0f, 0.0f, 0.0f), TEXT("HVAC_Fan"));
    CreateBox(FVector(770.0f, 470.0f, 110.0f), FVector(60.0f, 24.0f, 100.0f), FRotator::ZeroRotator, TEXT("Utility_Meter"));

    // Garden composition: stepping stones, planter beds, and simple tree proxies.
    for (int32 i = 0; i < 7; ++i)
    {
        CreateBox(FVector(-650.0f + i * 170.0f, -930.0f + (i % 2) * 35.0f, 10.0f), FVector(95.0f, 75.0f, 20.0f), FRotator::ZeroRotator, TEXT("Garden_StepStone"));
    }

    for (int32 i = 0; i < 4; ++i)
    {
        const float X = -1000.0f + i * 560.0f;
        CreateBox(FVector(X, 1320.0f, 30.0f), FVector(340.0f, 160.0f, 60.0f), FRotator::ZeroRotator, TEXT("Planter_Bed"));
        CreateCylinder(FVector(X - 80.0f, 1320.0f, 130.0f), FVector(14.0f, 14.0f, 220.0f), FRotator::ZeroRotator, TEXT("Tree_Trunk"));
        CreateSphere(FVector(X - 80.0f, 1320.0f, 300.0f), FVector(260.0f, 260.0f, 320.0f), FRotator::ZeroRotator, TEXT("Tree_Canopy"));
    }

    // Simple metal fence posts along the backyard boundary.
    for (int32 i = 0; i < 9; ++i)
    {
        CreateCylinder(FVector(-1050.0f + i * 260.0f, 1450.0f, 105.0f), FVector(5.0f, 5.0f, 210.0f), FRotator::ZeroRotator, TEXT("BackFence_Post"));
    }
}

void AShahdagMapGenerator::BuildInteriorDetailPass()
{
    // Small repeated props establish room scale without pretending to be final hero assets yet.
    CreateBox(FVector(660.0f, -330.0f, 16.0f), FVector(26.0f, 18.0f, 32.0f), FRotator::ZeroRotator, TEXT("Living_Remote"));
    CreateBox(FVector(-40.0f, -110.0f, 80.0f), FVector(18.0f, 18.0f, 36.0f), FRotator::ZeroRotator, TEXT("Coffee_Mug"));
    CreateBox(FVector(-470.0f, 350.0f, 230.0f), FVector(90.0f, 90.0f, 12.0f), FRotator::ZeroRotator, TEXT("Kitchen_Backsplash_Accent"));
    CreateBox(FVector(-770.0f, 300.0f, 230.0f), FVector(115.0f, 8.0f, 12.0f), FRotator::ZeroRotator, TEXT("Kitchen_Counter_SinkLip"));

    // Interior wall fixtures / small architectural details.
    for (int32 i = 0; i < 4; ++i)
    {
        CreateBox(FVector(660.0f, -500.0f + i * 85.0f, 170.0f), FVector(12.0f, 12.0f, 34.0f), FRotator::ZeroRotator, TEXT("Wall_Switch_Plate"));
    }

    // Bedroom wardrobe volumes and bathroom mirror proxy.
    CreateBox(FVector(-780.0f, 280.0f, UpperFloorZ + 110.0f), FVector(140.0f, 90.0f, 220.0f), FRotator::ZeroRotator, TEXT("Bedroom_A_Wardrobe"));
    CreateBox(FVector(-270.0f, 280.0f, UpperFloorZ + 110.0f), FVector(140.0f, 90.0f, 220.0f), FRotator::ZeroRotator, TEXT("Bedroom_B_Wardrobe"));
    CreateBox(FVector(510.0f, 540.0f, UpperFloorZ + 150.0f), FVector(8.0f, 130.0f, 160.0f), FRotator::ZeroRotator, TEXT("Bathroom_Mirror"));
}

void AShahdagMapGenerator::BuildLighting()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FTransform(FRotator(-42.0f, -28.0f, 0.0f)));
    if (Sun)
    {
        Sun->GetLightComponent()->SetIntensity(6.5f);
        Sun->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.93f, 0.84f));
        Sun->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    }

    ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FTransform(FRotator::ZeroRotator));
    if (Sky)
    {
        Sky->GetLightComponent()->SetIntensity(0.8f);
        Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    }

    AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FTransform(FRotator::ZeroRotator));
    if (Fog)
    {
        Fog->GetComponent()->FogDensity = 0.010f;
        Fog->GetComponent()->FogHeightFalloff = 0.28f;
        Fog->GetComponent()->DirectionalInscatteringExponent = 3.5f;
    }

    APostProcessVolume* Post = World->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FTransform(FRotator::ZeroRotator));
    if (Post)
    {
        Post->bUnbound = true;
        Post->Priority = 10.0f;
        Post->Settings.AutoExposureMinBrightness = 0.8f;
        Post->Settings.AutoExposureMaxBrightness = 1.1f;
        Post->Settings.BloomIntensity = 0.07f;
        Post->Settings.VignetteIntensity = 0.08f;
        Post->Settings.MotionBlurAmount = 0.0f;
    }

    // Street approach lights: practical sources will later be replaced by finished fixtures.
    for (int32 i = -4; i <= 4; ++i)
    {
        CreatePointLight(FVector(i * 1300.0f, -2870.0f, 500.0f), 1400.0f, FLinearColor(1.0f, 0.82f, 0.62f), 1000.0f);
    }
}

void AShahdagMapGenerator::BuildInteriorLights()
{
    // Warm practicals in the ground floor and cool utility contrast.
    CreatePointLight(FVector(-610, -250, 250), 950.0f, FLinearColor(1.0f, 0.72f, 0.50f), 520.0f);
    CreatePointLight(FVector(260, -260, 250), 1200.0f, FLinearColor(1.0f, 0.82f, 0.62f), 600.0f);
    CreatePointLight(FVector(-420, 350, 250), 800.0f, FLinearColor(0.86f, 0.90f, 1.0f), 460.0f);
    CreatePointLight(FVector(500, 360, 250), 700.0f, FLinearColor(0.88f, 0.92f, 1.0f), 400.0f);

    // Upper floor practicals.
    CreatePointLight(FVector(-650, -240, 560), 600.0f, FLinearColor(1.0f, 0.78f, 0.58f), 420.0f);
    CreatePointLight(FVector(-150, -240, 560), 600.0f, FLinearColor(1.0f, 0.78f, 0.58f), 420.0f);
    CreatePointLight(FVector(460, 350, 560), 650.0f, FLinearColor(0.90f, 0.94f, 1.0f), 440.0f);

    // Exterior tactical approach lights.
    CreateSpotLight(FVector(-220, -1450, 420), FRotator(-55, 90, 0), 1800.0f, FLinearColor(1.0f, 0.90f, 0.72f), 1100.0f, 20.0f, 42.0f);
    CreateSpotLight(FVector(1000, -900, 360), FRotator(-60, 130, 0), 1500.0f, FLinearColor(1.0f, 0.86f, 0.68f), 900.0f, 18.0f, 40.0f);
}
