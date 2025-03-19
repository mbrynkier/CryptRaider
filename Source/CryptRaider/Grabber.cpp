// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	//If Physics handle is null just return to avoid crashing
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent() != nullptr)
	{		
		FVector TargetLocantion = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocantion, GetComponentRotation());
	}
	

}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Display, TEXT("Released grabber"));
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	//If Physics handle is null just return to avoid crashing
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	UPrimitiveComponent* HitComponent = PhysicsHandle->GetGrabbedComponent();

	if (HitComponent != nullptr)
	{
		HitComponent->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
	
}

void UGrabber::Grab()
{	
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	//If Physics handle is null just return to avoid crashing
	if (PhysicsHandle == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	bool HasHit = GetGrabbableInReach(HitResult);

	if (HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();		

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent, 
			NAME_None, 
			HitResult.ImpactPoint, 
			GetComponentRotation());
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();	
	if (Result == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent."));
	}
	
	return Result;
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult)
{

	FVector Start = GetComponentLocation();
	FVector End = Start + (GetForwardVector() * MaxGrabDistance);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	DrawDebugSphere(GetWorld(), End, 10, 10 ,FColor::Blue, false, 5);

	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);	

	return GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, 
		FQuat::Identity, 
		ECC_GameTraceChannel2,
		Sphere
	);	
}