#include "instruction.h"
#include "program.h"
#include <cstdio>

namespace cslProgram
{
	EInstructionResult PrintInstruction::Execute(Program* context) const
	{
		printf("Executing print instruction\n");
		printf("%s\n", line.c_str());

		return EInstructionResult::Success;
	}

	EInstructionResult SetVarInstruction::Execute(Program* context) const
	{
		context->SetVar(name, value);
		return EInstructionResult::Success;
	}

	EInstructionResult RunFuncInstruction::Execute(Program* context) const
	{
		context->RunFunction(name);
		return EInstructionResult::Success;
	}
}