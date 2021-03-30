#pragma once

#include "ROSBridgeMsg.h"


namespace pr2_controllers_msgs
{
	class PR2GripperCommandResult : public FROSBridgeMsg
	{
		double Position;
		double Effort;
		bool Stalled;
		bool ReachedGoal;
	public:
		PR2GripperCommandResult()
		{
			MsgType = "pr2_controllers_msgs/PR2GripperCommandResult";
		}

		PR2GripperCommandResult
		(
			double InPosition,
			double InEffort,
			bool InStalled,
			bool InReachedGoal
		):
			Position(InPosition),
			Effort(InEffort),
			Stalled(InStalled),
			ReachedGoal(InReachedGoal)
		{
			MsgType = "pr2_controllers_msgs/PR2GripperCommandResult";
		}

		~PR2GripperCommandResult() override {}

		double GetPosition() const
		{
			return Position;
		}

		double GetEffort() const
		{
			return Effort;
		}

		bool GetStalled() const
		{
			return Stalled;
		}

		bool GetReachedGoal() const
		{
			return ReachedGoal;
		}

		void SetPosition(double InPosition)
		{
			Position = InPosition;
		}

		void SetEffort(double InEffort)
		{
			Effort = InEffort;
		}

		void SetStalled(bool InStalled)
		{
			Stalled = InStalled;
		}

		void SetReachedGoal(bool InReachedGoal)
		{
			ReachedGoal = InReachedGoal;
		}

		virtual void FromJson(TSharedPtr<FJsonObject> JsonObject) override
		{
			Position = JsonObject->GetNumberField(TEXT("position"));

			Effort = JsonObject->GetNumberField(TEXT("effort"));

			Stalled = JsonObject->GetBoolField(TEXT("stalled"));

			ReachedGoal = JsonObject->GetBoolField(TEXT("reached_goal"));

		}

		static PR2GripperCommandResult GetFromJson(TSharedPtr<FJsonObject> JsonObject)
		{
			PR2GripperCommandResult Result;
			Result.FromJson(JsonObject);
			return Result;
		}

		virtual TSharedPtr<FJsonObject> ToJsonObject() const override
		{
			TSharedPtr<FJsonObject> Object = MakeShareable<FJsonObject>(new FJsonObject());

			Object->SetNumberField(TEXT("position"), Position);
			Object->SetNumberField(TEXT("effort"), Effort);
			Object->SetBoolField(TEXT("stalled"), Stalled);
			Object->SetBoolField(TEXT("reached_goal"), ReachedGoal);
			return Object;
		}
		virtual FString ToYamlString() const override
		{
			FString OutputString;
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(ToJsonObject().ToSharedRef(), Writer);
			return OutputString;
		}
	};
}