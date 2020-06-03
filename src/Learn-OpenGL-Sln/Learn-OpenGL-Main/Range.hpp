#pragma once
#ifndef RANGE_HPP
#define RANGE_HPP
#include <exception>

struct Range
{
	float min = 0, max = INFINITY;
	Range(const float min, const float max)
	{
		if (min >= max) throw std::exception(
			"Invalid input minimum & maximum values. 'min' is greater or equal to 'max.'");

		this->min = min;
		this->max = max;
	}

	bool within(const float value) const
	{
		return value > min && value <= max;
	}
};
#endif