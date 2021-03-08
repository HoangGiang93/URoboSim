#include "Controller/RControllerCommands.h"
#include "Physics/RModel.h"
#include "Physics/RLink.h"

void URobotCommandsComponent::BeginPlay()
{
	Init();
}

void URobotCommandsComponent::Init()
{
	Model = Cast<ARModel>(GetOwner());
	if(!Model)
	{
		UE_LOG(LogTemp, Error, TEXT("RobotComandsComponent not attached to ARModel"));
	}
}

void URobotCommandsComponent::MoveWithVelocity(FVector InVelocity, float DeltaTime)
{
	for(auto& Link : Model->Links)
	{
		if(Link.Value->GetNumCollisions() > 0)
		{
			Link.Value->SetSimulatePhysics(false);
			FRotator Orientation = Link.Value->GetComponentRotation();
			FVector Position = Link.Value->GetComponentLocation();
			FVector DistanceTraveld = Orientation.Quaternion().RotateVector(InVelocity*DeltaTime);
			Link.Value->SetWorldLocation(DistanceTraveld + Position, false, nullptr, ETeleportType::TeleportPhysics);
			Link.Value->SetSimulatePhysics(true);
		}
	}
}
