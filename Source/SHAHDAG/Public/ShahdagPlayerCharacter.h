#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShahdagPlayerCharacter.generated.h"

class UCameraComponent;

UCLASS()
class SHAHDAG_API AShahdagPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AShahdagPlayerCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartSprint();
    void StopSprint();
    void ToggleCrouch();

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 250.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed = 430.0f;
};
