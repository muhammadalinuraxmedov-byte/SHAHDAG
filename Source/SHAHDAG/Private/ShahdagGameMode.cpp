#include "ShahdagGameMode.h"
#include "ShahdagPlayerCharacter.h"
#include "ShahdagMapGenerator.h"

#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

AShahdagGameMode::AShahdagGameMode()
{
    DefaultPawnClass = AShahdagPlayerCharacter::StaticClass();
}

void AShahdagGameMode::StartPlay()
{
    Super::StartPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    // Runtime-generated first playable map. This avoids requiring a binary .umap for the first milestone.
    World->SpawnActor<AShahdagMapGenerator>(AShahdagMapGenerator::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector));

    // Guaranteed start position for the first playable build.
    World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FTransform(FRotator(0, 0, 0), FVector(-1000.0f, -2600.0f, 140.0f)));
}
