#pragma once

#ifndef CSLPROGRAM_VARIABLE_H
#define CSLPROGRAM_VARIABLE_H

namespace cslProgram
{
	class Variable
	{
	private:
		int data = 0;

	public:

		Variable(int inData)
		{
			data = inData;
		}

		int GetData() { return data; }
	};
}

#endif