#pragma once

#ifndef CSLPROGRAM_INSTRUCTION_H
#define CSLPROGRAM_INSTRUCTION_H

namespace cslProgram
{
	class Program;

	class Instruction
	{
	public:
		enum EResult
		{
			Fail,
			Success,
			CondFail,
			CondSuccess
		};

		Instruction() {}

		virtual EResult Execute(const Program* context) const = 0;
	};

	class PrintInstruction : public Instruction
	{
	private:
		const char* printParam;

	public:
		PrintInstruction(const char* inParam)
		{
			printParam = inParam;
		}

		virtual EResult Execute(const Program* context) const override;
	};
}

#endif