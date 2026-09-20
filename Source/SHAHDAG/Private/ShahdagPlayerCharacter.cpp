#include "ShahdagPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "Engine/Engine.h"

AShahdagPlayerCharacter::AShahdagPlayerCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->MaxAcceleration = 1600.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 1400.0f;
    GetCharacterMovement()->GroundFriction = 8.0f;
    GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
    GetCharacterMovement()->AirControl = 0.15f;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AShahdagPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShahdagPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AShahdagPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AShahdagPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AShahdagPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AShahdagPlayerCharacter::LookUp);

    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AShahdagPlayerCharacter::StartSprint);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AShahdagPlayerCharacter::StopSprint);
    PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AShahdagPlayerCharacter::ToggleCrouch);
}

void AShahdagPlayerCharacter::MoveForward(float Value)
{
    if (FMath::IsNearlyZero(Value)) return;
    AddMovementInput(GetActorForwardVector(), Value);
}

void AShahdagPlayerCharacter::MoveRight(float Value)
{
    if (FMath::IsNearlyZero(Value)) return;
    AddMovementInput(GetActorRightVector(), Value);
}

void AShahdagPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AShahdagPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AShahdagPlayerCharacter::StartSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AShahdagPlayerCharacter::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShahdagPlayerCharacter::ToggleCrouch()
{
    if (bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }
}
