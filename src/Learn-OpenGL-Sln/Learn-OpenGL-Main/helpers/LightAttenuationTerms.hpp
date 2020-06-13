#pragma once
#ifndef LIGHT_ATTENUATION_TERMS_HPP
#define LIGHT_ATTENUATION_TERMS_HPP

#include <vector>

class LightAttenuationTerms
{
public:
	struct AttenuationTerms
	{
		float constant = 1, linear = 0.7f, quadratic = 1.8f;

		AttenuationTerms(const float constant, const float linear, const float quadratic)
			: constant(constant), linear(linear), quadratic(quadratic) { }
	};

	LightAttenuationTerms()
	{
		// http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		//Distance	Constant	Linear	Quadratic
		//	7	1.0	0.7	1.8
		//	13	1.0	0.35	0.44
		//	20	1.0	0.22	0.20
		//	32	1.0	0.14	0.07
		//	50	1.0	0.09	0.032
		//	65	1.0	0.07	0.017
		//	100	1.0	0.045	0.0075
		//	160	1.0	0.027	0.0028
		//	200	1.0	0.022	0.0019
		//	325	1.0	0.014	0.0007
		//	600	1.0	0.007	0.0002
		//	3250	1.0	0.0014	0.000007

		add(7,    1, 0.7f,    1.8f);
		add(13,   1, 0.35f,   0.44f);
		add(20,   1, 0.22f,   0.2f);
		add(32,   1, 0.14f,   0.07f);
		add(50,   1, 0.09f,   0.032f);
		add(65,   1, 0.07f,   0.017f);
		add(100,  1, 0.045f,  0.0075f);
		add(160,  1, 0.027f,  0.0028f);
		add(200,  1, 0.022f,  0.0019f);
		add(325,  1, 0.014f,  0.0007f);
		add(600,  1, 0.007f,  0.0002f);
		add(3250, 1, 0.0014f, 0.000007f);
	}

	AttenuationTerms getAttenuation(const float distance)
	{
		for (auto iterator = _lightAttenuation.begin();
			iterator != _lightAttenuation.end();
			++iterator)
		{
			const auto attenuation = *iterator;
			if (distance <= attenuation.maxRange) return attenuation.term;
		}

		return (*_lightAttenuation.cend()).term;
	}

private:
	struct LightAttenuation
	{
		float maxRange = INFINITY;
		AttenuationTerms term;

		LightAttenuation(const float maxRange, const AttenuationTerms& term)
			: maxRange(maxRange), term(term) {}
	};

	std::vector<LightAttenuation> _lightAttenuation;

	void add(const float r, const float c, const float l, const float q)
	{
		_lightAttenuation.emplace_back(r, AttenuationTerms(c, l, q));
	}
};

#endif