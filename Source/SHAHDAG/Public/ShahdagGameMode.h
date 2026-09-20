#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShahdagGameMode.generated.h"

UCLASS()
class SHAHDAG_API AShahdagGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AShahdagGameMode();

protected:
    virtual void StartPlay() override;
};
