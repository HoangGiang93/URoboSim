#include "Controller/ControllerType/BaseController/ROmniwheelController.h"

UROmniwheelController::UROmniwheelController()
{
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
      WheelSetting.WheelVelocities[0] = (LinearVelocity.X * 100 + LinearVelocity.Y * 100 + WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelSetting.WheelVelocities[1] = (LinearVelocity.X * 100 - LinearVelocity.Y * 100 - WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelSetting.WheelVelocities[2] = (LinearVelocity.X * 100 - LinearVelocity.Y * 100 + WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;
      WheelSetting.WheelVelocities[3] = (LinearVelocity.X * 100 + LinearVelocity.Y * 100 - WheelSetting.WheelToCenterSum * AngularVelocity) / WheelSetting.WheelRadius;

      FVector RotationAxis = GetOwner()->GetBaseLink()->GetRootMesh()->GetComponentQuat().GetAxisY();
      GetOwner()->GetLink(WheelSetting.WheelFrontLeft)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelSetting.WheelVelocities[0]);
      GetOwner()->GetLink(WheelSetting.WheelFrontRight)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelSetting.WheelVelocities[1]);
      GetOwner()->GetLink(WheelSetting.WheelBackLeft)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelSetting.WheelVelocities[2]);
      GetOwner()->GetLink(WheelSetting.WheelBackRight)->GetRootMesh()->SetPhysicsAngularVelocityInDegrees(RotationAxis * WheelSetting.WheelVelocities[3]);
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("%s can not find omni wheels in %s"), *GetName(), *GetOwner()->GetName())
    }
  }
}
