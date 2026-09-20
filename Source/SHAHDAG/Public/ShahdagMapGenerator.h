#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShahdagMapGenerator.generated.h"

UCLASS()
class SHAHDAG_API AShahdagMapGenerator : public AActor
{
    GENERATED_BODY()

public:
    AShahdagMapGenerator();

protected:
    virtual void BeginPlay() override;

private:
    void BuildEnvironment();
    void BuildStreetAndBoundaries();
    void BuildResidentialHouse();
    void BuildExteriorContext();
    void BuildInteriorRooms();
    void BuildFurnitureAndProps();
    void BuildLighting();
    void BuildInteriorLights();
    void BuildAtmosphereAndArtDirection();
    void BuildStreetFurnitureAndUtilities();
    void BuildResidentialExteriorDetails();
    void BuildInteriorDetailPass();

    AActor* CreateBox(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label);
    AActor* CreateCylinder(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label);
    AActor* CreateSphere(const FVector& LocationCm, const FVector& SizeCm, const FRotator& Rotation, const FString& Label);
    void ApplyPrimitiveLook(class UStaticMeshComponent* Mesh, const FString& Label);
    AActor* CreatePointLight(const FVector& LocationCm, float Intensity, const FLinearColor& Color, float Radius);
    AActor* CreateSpotLight(const FVector& LocationCm, const FRotator& Rotation, float Intensity, const FLinearColor& Color, float Radius, float InnerCone, float OuterCone);
    void CreateWallSegment(const FVector& A, const FVector& B, float Height, float Thickness, float ZCenter, const FString& Label);
    void CreateDoorFrame(const FVector& Center, float Width, float Height, float WallThickness, const FString& Label);
    void CreateWindowFrame(const FVector& Center, float Width, float Height, float WallThickness, const FString& Label);
    void CreateTable(const FVector& Center, const FVector& Size, const FString& Label);
    void CreateChair(const FVector& Center, const FRotator& Rotation, const FString& Label);
    void CreateBed(const FVector& Center, const FVector& Size, const FRotator& Rotation, const FString& Label);
    void CreateCabinetRun(const FVector& Start, int32 Count, float Spacing, const FString& LabelPrefix);

    UPROPERTY(EditAnywhere, Category = "SHAHDAG|Map")
    bool bBuildOnBeginPlay = true;
};
