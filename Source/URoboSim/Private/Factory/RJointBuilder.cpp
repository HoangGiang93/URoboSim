#include "Factory/RJointBuilder.h"
#include "Conversions.h"
#include "Physics/RLink.h"

DEFINE_LOG_CATEGORY_STATIC(LogRJointBuilder, Log, All);

/* End helper functions */

bool URJointBuilder::LoadSDF(USDFJoint *&SDFJoint)
{
  if (!SDFJoint)
  {
    return false;
  }
  else
  {
    SetNewJoint(SDFJoint);
    return true;
  }
}

void URContinuousJointBuilder::SetNewJoint(USDFJoint *&SDFJoint)
{
  Joint = NewObject<URContinuousJoint>(Model, *SDFJoint->GetName());
  URJointBuilder::SetNewJoint(SDFJoint);
}

void URRevoluteJointBuilder::SetNewJoint(USDFJoint *&SDFJoint)
{
  Joint = NewObject<URRevoluteJoint>(Model, *SDFJoint->GetName());
  URJointBuilder::SetNewJoint(SDFJoint);
}

void URPrismaticJointBuilder::SetNewJoint(USDFJoint *&SDFJoint)
{
  Joint = NewObject<URPrismaticJoint>(Model, *SDFJoint->GetName());
  URJointBuilder::SetNewJoint(SDFJoint);
}

void URJointBuilder::SetNewJoint(USDFJoint *&SDFJoint)
{
  if (Joint)
  {
    SetJointParameters(SDFJoint);
    URLink *Parent = nullptr;
    URLink *Child = nullptr;
    for (URLink *&Link : Model->GetLinks())
    {
      if (Link->GetName().Equals(SDFJoint->Parent))
      {
        Parent = Link;
      }
      else if (Link->GetName().Equals(SDFJoint->Child))
      {
        Child = Link;
      }
    }
    if (!Parent)
    {
      UE_LOG(LogRJointBuilder, Error, TEXT("ParentLink %s of Joint %s not found"), *SDFJoint->Parent, *SDFJoint->GetName())
      return;
    }
    if (!Child)
    {
      UE_LOG(LogRJointBuilder, Error, TEXT("ChildLink %s of Joint %s not found"), *SDFJoint->Child, *SDFJoint->GetName())
      return;
    }
    Child->AttachToComponent(Parent->GetRootMesh());
    Joint->SetParentChild(Parent, Child);
    CreateConstraint(SDFJoint);
    Model->AddJoint(Joint);
  }
  else
  {
    UE_LOG(LogRJointBuilder, Error, TEXT("Joint %s not initialized"), *SDFJoint->GetName())
  }
}

void URJointBuilder::SetJointParameters(USDFJoint *&SDFJoint)
{
  Joint->SetJointType(SDFJoint);
}

void URJointBuilder::CreateConstraint(USDFJoint *&SDFJoint)
{
  UPhysicsConstraintComponent *Constraint;
  Constraint = NewObject<UPhysicsConstraintComponent>(Joint);
  Constraint->ConstraintInstance.SetDisableCollision(true);
  Constraint->ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
  Constraint->ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
  Constraint->ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
  Constraint->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
  Constraint->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
  Constraint->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
  Constraint->ConstraintInstance.AngularRotationOffset = FRotator(0.f);
  Constraint->ConstraintInstance.ProfileInstance.TwistLimit.bSoftConstraint = false;
  Constraint->ConstraintInstance.ProfileInstance.ConeLimit.bSoftConstraint = false;

  SetupConstraint(Constraint, SDFJoint->Axis);
  ConnectToComponents(Constraint, SDFJoint);
  Joint->SetConstraint(Constraint);
}

void URContinuousJointBuilder::SetupConstraint(UPhysicsConstraintComponent *&Constraint, USDFJointAxis *&SDFJointAxis)
{
  if (SDFJointAxis->Xyz.Equals(FVector::ForwardVector))
  {
    Constraint->ConstraintInstance.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Free);
  }
  else if (SDFJointAxis->Xyz.Equals(FVector::RightVector))
  {
    Constraint->ConstraintInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Free);
  }
  else
  {
    Constraint->ConstraintInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Free);
  }
}

void URRevoluteJointBuilder::SetupConstraint(UPhysicsConstraintComponent *&Constraint, USDFJointAxis *&SDFJointAxis)
{
  //Because the limit is symetrical the rotation center has to be offseted so that upper and lower limit correspond to the sdf values
  float JointLimit = 0.5 * FMath::Abs(SDFJointAxis->Upper - SDFJointAxis->Lower);
  float RotationOffset = CalculateRotationOffset(JointLimit, SDFJointAxis);
  Joint->SetOffsetPosition(FMath::RadiansToDegrees(RotationOffset));

  if (SDFJointAxis->Xyz.Equals(FVector::ForwardVector))
  {
    Constraint->ConstraintInstance.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Free);
  }
  else if (SDFJointAxis->Xyz.Equals(FVector::RightVector))
  {
    Constraint->ConstraintInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Free);
  }
  else
  {
    Constraint->ConstraintInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Free);
  }
}

const float URRevoluteJointBuilder::CalculateRotationOffset(float &JointLimit, USDFJointAxis *&SDFJointAxis)
{
  if (JointLimit > 2 * PI)
  {
    JointLimit = PI;
    return 0;
  }
  else
  {
    return -0.5 * (SDFJointAxis->Upper + SDFJointAxis->Lower);
  }
}

void URPrismaticJointBuilder::SetupConstraint(UPhysicsConstraintComponent *&Constraint, USDFJointAxis *&SDFJointAxis)
{
  //Because the limit is symetrical the rotation center has to be offseted so that upper and lower limit correspond to the sdf values
  Joint->SetOffsetPosition(FConversions::MToCm((float)0.5 * FMath::Abs(SDFJointAxis->Upper + SDFJointAxis->Lower)));

  if (FMath::Abs(SDFJointAxis->Lower) < 100 && FMath::Abs(SDFJointAxis->Upper) < 100)
  {
    if (SDFJointAxis->Xyz.Equals(FVector::ForwardVector))
    {
      Constraint->ConstraintInstance.SetLinearXMotion(ELinearConstraintMotion::LCM_Free);
    }
    else if (SDFJointAxis->Xyz.Equals(FVector::RightVector))
    {
      Constraint->ConstraintInstance.SetLinearYMotion(ELinearConstraintMotion::LCM_Free);
    }
    else
    {
      Constraint->ConstraintInstance.SetLinearZMotion(ELinearConstraintMotion::LCM_Free);
    }
  }
}

void URJointBuilder::ConnectToComponents(UPhysicsConstraintComponent *&Constraint, USDFJoint *&SDFJoint)
{
  if (Constraint)
  {
    UStaticMeshComponent *ParentRootMesh = Joint->GetParent()->GetRootMesh();
    UStaticMeshComponent *ChildRootMesh = Joint->GetChild()->GetRootMesh();
    if (!ParentRootMesh)
    {
      UE_LOG(LogRJointBuilder, Error, TEXT("Parent of %s not found"), *Joint->GetName())
      return;
    }
    if (!ChildRootMesh)
    {
      UE_LOG(LogRJointBuilder, Error, TEXT("Child of %s not found"), *Joint->GetName())
      return;
    }
    Constraint->RegisterComponent();
    Constraint->AttachToComponent(ParentRootMesh, FAttachmentTransformRules::KeepWorldTransform);
    if (SDFJoint->Axis->bUseParentModelFrame)
    {
      Constraint->SetWorldLocation(Joint->GetChild()->GetPose().GetLocation());
      Constraint->AddLocalOffset(SDFJoint->Pose.GetLocation());
    }
    else
    {
      Constraint->SetWorldLocation(SDFJoint->Pose.GetLocation());
    }
    RotateConstraintToRefAxis(Constraint, SDFJoint->Axis);
    Constraint->ConstraintActor1 = ParentRootMesh->GetOwner();
    Constraint->ConstraintActor2 = ChildRootMesh->GetOwner();
    Constraint->SetConstrainedComponents(Cast<UPrimitiveComponent>(ParentRootMesh), NAME_None, Cast<UPrimitiveComponent>(ChildRootMesh), NAME_None);
  }
  else
  {
    UE_LOG(LogRJointBuilder, Error, TEXT("Joint %s doesn't have constraint, ConnectToComponents failed"), *Joint->GetName())
  }
}

void URJointBuilder::RotateConstraintToRefAxis(UPhysicsConstraintComponent *&Constraint, USDFJointAxis *&SDFJointAxis)
{
  if (Constraint)
  {
    if (SDFJointAxis->Xyz.Equals(FVector::ForwardVector) || SDFJointAxis->Xyz.Equals(FVector::RightVector) || SDFJointAxis->Xyz.Equals(FVector::UpVector))
    {
      Joint->SetJointAxis(SDFJointAxis->Xyz);
    }
    else
    {
      Joint->SetJointAxis(FVector::UpVector);
      FQuat BetweenQuat = FQuat::FindBetweenVectors(FVector::UpVector, SDFJointAxis->Xyz);
      Constraint->SetRelativeRotationExact(Constraint->GetRelativeRotationFromWorld(BetweenQuat).Rotator());
    }
  }
  else
  {
    UE_LOG(LogRJointBuilder, Error, TEXT("Joint %s doesn't have constraint, RotateConstraintToRefAxis failed"), *Joint->GetName());
  }
}