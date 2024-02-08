#include "instruction.h"
#include <cstdio>

namespace cslProgram
{
	Instruction::EResult PrintInstruction::Execute(const Program* context) const
	{
		if (printParam == nullptr)
		{
			Instruction::EResult::Fail;
		}

		printf("Executing print instruction\n");
		printf("%s\n", printParam);

		return Instruction::EResult::Success;
	}
}