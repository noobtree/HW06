// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW06PickUpComponent.h"

UHW06PickUpComponent::UHW06PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UHW06PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UHW06PickUpComponent::OnSphereBeginOverlap);
}

void UHW06PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AHW06Character* Character = Cast<AHW06Character>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
