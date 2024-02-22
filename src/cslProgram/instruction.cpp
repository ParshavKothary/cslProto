#include "instruction.h"

#include "common/common.h"
#include "program.h"

#include <cstdio>

namespace cslProgram
{
	EInstructionResult PrintInstruction::Execute(Program* context) const
	{
		std::string totalLine = "";
		for (const std::string& word : line)
		{
			std::string value = word;
			context->GetValueFromValueOrName(value);
			totalLine += value;
		}
		PRINTF("%s\n", totalLine.c_str());

		return EInstructionResult::Success;
	}

	EInstructionResult SetVarInstruction::Execute(Program* context) const
	{
		context->SetVar(name, value);
		return EInstructionResult::Success; // cannot fail \m/
	}

	EInstructionResult RunFuncInstruction::Execute(Program* context) const
	{
		if (context->RunFunction(name) == false)
		{
			PRINTF("Runtime Error: Run function failed at line: %s\n", srcLine.c_str());
			return EInstructionResult::Fail;
		}
		return EInstructionResult::Success;
	}

	EInstructionResult IsGreaterConditional::Execute(Program* context) const
	{
		float lVal;
		if (context->GetFloatFromValueOrName(lVar, lVal) == false)
		{
			PRINTF("Runtime Error: Failed to get value from argument %s in line: %s\n", lVar.c_str(), srcLine.c_str());
			return EInstructionResult::Fail;
		}

		float rVal;
		if (context->GetFloatFromValueOrName(rVar, rVal) == false)
		{
			PRINTF("Runtime Error: Failed to get value from argument %s in line: %s\n", rVar.c_str(), srcLine.c_str());
			return EInstructionResult::Fail;
		}

		return lVal > rVal ? EInstructionResult::CondTrue : EInstructionResult::CondFalse;
	}

	EInstructionResult IsGreaterEqualConditional::Execute(Program* context) const
	{
		float lVal;
		if (context->GetFloatFromValueOrName(lVar, lVal) == false)
		{
			PRINTF("Runtime Error: Failed to get value from argument %s in line: %s\n", lVar.c_str(), srcLine.c_str());
			return EInstructionResult::Fail;
		}

		float rVal;
		if (context->GetFloatFromValueOrName(rVar, rVal) == false)
		{
			PRINTF("Runtime Error: Failed to get value from argument %s in line: %s\n", rVar.c_str(), srcLine.c_str());
			return EInstructionResult::Fail;
		}

		return lVal >= rVal ? EInstructionResult::CondTrue : EInstructionResult::CondFalse;
	}
}