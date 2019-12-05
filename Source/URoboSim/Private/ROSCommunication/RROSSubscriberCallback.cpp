#include "ROSCommunication/RROSSubscriberCallback.h"
// #include "sensor_msgs/JointState.h"
#include "Conversions.h"
#include "control_msgs/FollowJointTrajectoryActionGoal.h"
#include "pr2_controllers_msgs/PR2GripperCommandActionGoal.h"
#include "pr2_controllers_msgs/PR2PointHeadActionGoal.h"
#include "urobosim_msgs/PerceiveObjectActionGoal.h"
#include "geometry_msgs/Twist.h"

FROSWholeBodyControllerCallback::FROSWholeBodyControllerCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	JointController = Cast<URJointController>(InController);
}

FROSFollowTrajectoryActionCancelCallback::FROSFollowTrajectoryActionCancelCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	JointController = Cast<URJointController>(InController);
}

FROSActionCancelCallback::FROSActionCancelCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	Controller = Cast<URController>(InController);
}

FROSWholeBodyControllerCallback::~FROSWholeBodyControllerCallback()
{
}

FROSFollowTrajectoryActionCancelCallback::~FROSFollowTrajectoryActionCancelCallback()
{
}

FROSActionCancelCallback::~FROSActionCancelCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSWholeBodyControllerCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> JointStateMessage =
		MakeShareable<control_msgs::FollowJointTrajectoryActionGoal>(new control_msgs::FollowJointTrajectoryActionGoal());

	JointStateMessage->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(JointStateMessage);
}

void FROSWholeBodyControllerCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(JointController)
	{
		TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> TrajectoryMsg = StaticCastSharedPtr<control_msgs::FollowJointTrajectoryActionGoal>(Msg);

		TArray<FString> Names =TrajectoryMsg->GetGoal().GetTrajectory().GetJointNames();
		JointController->SetJointNames(Names);

		actionlib_msgs::GoalID Id = TrajectoryMsg->GetGoalId();
        UE_LOG(LogTemp, Log, TEXT("Recieved TrajectoryCommand Id: %s"), *Id.GetId());
		JointController->GoalStatusList.Add(FGoalStatusInfo(Id.GetId(), Id.GetStamp().Secs, Id.GetStamp().NSecs));
		for(auto& JointPoint : TrajectoryMsg->GetGoal().GetTrajectory().GetPoints())
		{
			FTrajectoryPoints TempPoints;
			// FTrajectoryPoints TempError;
			TempPoints.Points = JointPoint.GetPositions();
			FROSTime TimeStep(JointPoint.GetTimeFromStart());
			TempPoints.SetTimeStep(TimeStep.Secs, TimeStep.NSecs);
			JointController->Trajectory.Add(TempPoints) ;
			// JointController->Error.Add(TempError) ;
		}
		// JointController->bFollowTrajectory = true;
                JointController->FollowTrajectory();
		UE_LOG(LogTemp, Error, TEXT("Call Callback for TrajectoryGoal"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JonintController not found"));
	}
}

void FROSActionCancelCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(Controller)
	{
          Controller->bCancel = true;
        }
	else
	{
          UE_LOG(LogTemp, Error, TEXT("JonintController not found"));
	}
}

void FROSFollowTrajectoryActionCancelCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(JointController)
	{
		UE_LOG(LogTemp, Error, TEXT("The FollowJointTrajectoryActionCancel Callback is a dummy and not implemented yet"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JonintController not found"));
	}
}

TSharedPtr<FROSBridgeMsg> FROSActionCancelCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<actionlib_msgs::GoalID> GoalId =
		MakeShareable<actionlib_msgs::GoalID>(new actionlib_msgs::GoalID());

	GoalId->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(GoalId);
}

TSharedPtr<FROSBridgeMsg> FROSFollowTrajectoryActionCancelCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<actionlib_msgs::GoalID> GoalId =
		MakeShareable<actionlib_msgs::GoalID>(new actionlib_msgs::GoalID());

	GoalId->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(GoalId);
}

FROSBaseControllerCallback::FROSBaseControllerCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	BaseController = Cast<URBaseController>(InController);
}

FROSBaseControllerCallback::~FROSBaseControllerCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSBaseControllerCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> JointStateMessage =
		MakeShareable<control_msgs::FollowJointTrajectoryActionGoal>(new control_msgs::FollowJointTrajectoryActionGoal());

	JointStateMessage->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(JointStateMessage);
}

void FROSBaseControllerCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	// if(BaseController)
	// {
	// 	TSharedPtr<control_msgs::FollowJointTrajectoryActionGoal> TrajectoryMsg = StaticCastSharedPtr<control_msgs::FollowJointTrajectoryActionGoal>(Msg);
    //
	// 	TArray<FString> Names =TrajectoryMsg->GetGoal().GetTrajectory().GetJointNames();
	// 	BaseController->SetJointNames(Names);
	// 	for(auto& JointPoint : TrajectoryMsg->GetGoal().GetTrajectory().GetPointss())
	// 	{
	// 		FTrajectoryPoints TempPoints;
	// 		TempPoints.Points = JointPoint.GetPositions();
	// 		BaseController->Trajectory.Add(TempPoints) ;
	// 	}
	// 	BaseController->bFollowTrajectory = true;
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("JonintController not found"));
}

FROSVelocityCommandSubscriberCallback::FROSVelocityCommandSubscriberCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	BaseController = Cast<URBaseController>(InController);
}

FROSVelocityCommandSubscriberCallback::~FROSVelocityCommandSubscriberCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSVelocityCommandSubscriberCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<geometry_msgs::Twist> VelocityCommand =
		MakeShareable<geometry_msgs::Twist>(new geometry_msgs::Twist());

	VelocityCommand->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(VelocityCommand);
}

void FROSVelocityCommandSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(BaseController)
	{
		TSharedPtr<geometry_msgs::Twist> VelocityCommand = StaticCastSharedPtr<geometry_msgs::Twist>(Msg);
		FVector Linear = FConversions::ROSToU(VelocityCommand->GetLinear().GetVector());
		FVector Angular = VelocityCommand->GetAngular().GetVector();
		BaseController->MoveLinear(Linear);
		BaseController->Turn(Angular.Z * 180. / PI);
	}
}


FROSPointHeadGoalSubscriberCallback::FROSPointHeadGoalSubscriberCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	Controller = Cast<URPR2HeadTrajectoryController>(InController);
}

FROSPointHeadGoalSubscriberCallback::~FROSPointHeadGoalSubscriberCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSPointHeadGoalSubscriberCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<pr2_controllers_msgs::PR2PointHeadActionGoal> Message =
		MakeShareable<pr2_controllers_msgs::PR2PointHeadActionGoal>(new pr2_controllers_msgs::PR2PointHeadActionGoal());

	Message->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(Message);
}

void FROSPointHeadGoalSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(Controller)
	{
		TSharedPtr<pr2_controllers_msgs::PR2PointHeadActionGoal> Command = StaticCastSharedPtr<pr2_controllers_msgs::PR2PointHeadActionGoal>(Msg);
		Controller->Point = FConversions::ROSToU(Command->GetGoal().GetTarget().GetPoint().GetVector());
		Controller->FrameId = Command->GetGoal().GetTarget().GetHeader().GetFrameId();
		Controller->PointingFrame = Command->GetGoal().GetPointingFrame();
		Controller->Axis = Command->GetGoal().GetPointingAxis().GetVector();

		actionlib_msgs::GoalID Id = Command->GetGoalId();
		Controller->GoalStatusList.Add(FGoalStatusInfo(Id.GetId(), Id.GetStamp().Secs, Id.GetStamp().NSecs));
        UE_LOG(LogTemp, Log, TEXT("Recieved HeadAction Id: %s Goal: %s"), *Id.GetId(), *Controller->Point.ToString());
        AsyncTask(ENamedThreads::GameThread, [this]()
        {
           Controller->UpdateHeadDirection();
            Controller->bActive = true;
        }
        );
	}
}

FROSGripperCommandSubscriberCallback::FROSGripperCommandSubscriberCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	Controller = Cast<URGripperController>(InController);
}

FROSGripperCommandSubscriberCallback::~FROSGripperCommandSubscriberCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSGripperCommandSubscriberCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal> Message =
		MakeShareable<pr2_controllers_msgs::PR2GripperCommandActionGoal>(new pr2_controllers_msgs::PR2GripperCommandActionGoal());

	Message->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(Message);
}

void FROSGripperCommandSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
  if(Controller)
    {
      TSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal> Command = StaticCastSharedPtr<pr2_controllers_msgs::PR2GripperCommandActionGoal>(Msg);

      actionlib_msgs::GoalID Id = Command->GetGoalId();

      AsyncTask(ENamedThreads::GameThread, [this, Id, Command]()
                {
                  Controller->Position = Command->GetGoal().GetCommand().GetPosition() * 100.0;
                  Controller->MaxEffort = Command->GetGoal().GetCommand().GetMaxEffort();
                  UE_LOG(LogTemp, Log, TEXT("Recieved GripperCommand Id: %s Goal: %f"), *Id.GetId(), Controller->Position);
                  Controller->GoalStatusList.Add(FGoalStatusInfo(Id.GetId(), Id.GetStamp().Secs, Id.GetStamp().NSecs));
                  Controller->UpdateGripper();
                }
                );
    }

}

FROSPerceiveObjectGoalSubscriberCallback::FROSPerceiveObjectGoalSubscriberCallback(
	const FString& InTopic, const FString& InType, UObject* InController) :
	FROSBridgeSubscriber(InTopic, InType)
{
	Controller = Cast<URCameraController>(InController);
}

FROSPerceiveObjectGoalSubscriberCallback::~FROSPerceiveObjectGoalSubscriberCallback()
{
}

TSharedPtr<FROSBridgeMsg> FROSPerceiveObjectGoalSubscriberCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<urobosim_msgs::PerceiveObjectActionGoal> Message =
		MakeShareable<urobosim_msgs::PerceiveObjectActionGoal>(new urobosim_msgs::PerceiveObjectActionGoal());

	Message->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(Message);
}

void FROSPerceiveObjectGoalSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	if(Controller)
	{
		TSharedPtr<urobosim_msgs::PerceiveObjectActionGoal> Command = StaticCastSharedPtr<urobosim_msgs::PerceiveObjectActionGoal>(Msg);
		actionlib_msgs::GoalID Id = Command->GetGoalId();
		Controller->GoalStatusList.Add(FGoalStatusInfo(Id.GetId(), Id.GetStamp().Secs, Id.GetStamp().NSecs));
        Controller->TypeToPerceive = Command->GetGoal().GetType();
		Controller->bActive = true;
        AsyncTask(ENamedThreads::GameThread, [this]()
        {
          // // Controller->Camera->UpdateImage();
          Controller->PerceiveObject();
        }
        );

	}
}