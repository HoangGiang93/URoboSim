#include "Controller/ControllerType/BaseController/ROmniwheelController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogROmniwheelController, Log, All)

UROmniwheelController::UROmniwheelController()
{
  WheelVelocities.Init(0.0, 4);
}

void UROmniwheelController::SetControllerParameters(URControllerParameter *&ControllerParameters)
{
  UROmniwheelControllerParameter *OmniWheelControllerParameters = Cast<UROmniwheelControllerParameter>(ControllerParameters);
  if (OmniWheelControllerParameters)
  {
    Super::SetControllerParameters(ControllerParameters);
    WheelSetting = OmniWheelControllerParameters->WheelSetting;
  }
}

void UROmniwheelController::Tick(const float &InDeltaTime)
{
  Super::Tick(InDeltaTime);
  MoveWheelTick(InDeltaTime);
}

void UROmniwheelController::MoveWheelTick(const float &InDeltaTime)
{
  if (GetOwner())
  {
    if (GetOwner()->GetLink(WheelSetting.WheelFrontLeft) &&
        GetOwner()->GetLink(WheelSetting.WheelBackLeft) &&
        GetOwner()->GetLink(WheelSetting.WheelFrontRight) &&
        GetOwner()->GetLink(WheelSetting.WheelBackRight))
    {
      WheelVelocities[0] = (LinearVelocity.X * 100 + LinearVelocity.Y * 100 + WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelVelocities[1] = (LinearVelocity.X * 100 - LinearVelocity.Y * 100 - WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelVelocities[2] = (LinearVelocity.X * 100 - LinearVelocity.Y * 100 + WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelVelocities[3] = (LinearVelocity.X * 100 + LinearVelocity.Y * 100 - WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;

      FVector RotationAxis = GetOwner()->GetBaseLink()->GetRootMesh()->GetComponentQuat().GetAxisY();
      GetOwner()->GetLink(WheelSetting.WheelFrontLeft)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelVelocities[0]);
      GetOwner()->GetLink(WheelSetting.WheelFrontRight)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelVelocities[1]);
      GetOwner()->GetLink(WheelSetting.WheelBackLeft)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelVelocities[2]);
      GetOwner()->GetLink(WheelSetting.WheelBackRight)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelVelocities[3]);
    }
    else
    {
      UE_LOG(LogROmniwheelController, Error, TEXT("Omniwheels in %s not found"), *GetOwner()->GetName())
    }
  }
}
