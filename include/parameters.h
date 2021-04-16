#pragma once

namespace Steinberg {
namespace TransientShaper {

	const double kAttackMin				= 1;		// ms
	const double kAttackMax				= 500;		// ms
	const double kReleaseMin			= 1;		// ms
	const double kReleaseMax			= 500;		// ms
	const double kTransientFactorMin	= 0.0;		// dB
	const double kTransientFactorMax	= 10.0;		// dB
	const double kGainMin				= -20.0;	// dB
	const double kGainMax				= 20.0;		// dB

	inline double ParameterToPlain(double normalized, double min, double max)
	{
		return normalized * (max - min) + min;
	}

	inline double ParameterToNormalized(double plain, double min, double max)
	{
		return (plain - min) / (max - min);
	}

	inline float Clip(float val, float min, float max)
	{
		return std::min<float>(std::max<float>(val, min), max);
	}

}
}