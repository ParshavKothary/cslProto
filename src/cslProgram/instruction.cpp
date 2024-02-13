#include "instruction.h"

#include "common/common.h"
#include "program.h"

#include <cstdio>

namespace cslProgram
{
	EInstructionResult PrintInstruction::Execute(Program* context) const
	{
		PRINTF("Executing print instruction\n");
		PRINTF("%s\n", line.c_str());

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
			return EInstructionResult::Fail;
		}
		return EInstructionResult::Success;
	}

	EInstructionResult IsGreaterConditional::Execute(Program* context) const
	{
		float lVal;
		if (context->GetFloatFromValueOrName(lVar, lVal) == false)
		{
			return EInstructionResult::Fail;
		}

		float rVal;
		if (context->GetFloatFromValueOrName(rVar, rVal) == false)
		{
			return EInstructionResult::Fail;
		}

		return lVal > rVal ? EInstructionResult::CondTrue : EInstructionResult::CondFalse;
	}
}