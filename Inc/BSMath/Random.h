#pragma once

#include <random>

namespace BSMath
{
	namespace Detail
	{
		class RandomBase
		{
		protected:
			static std::random_device randomDevice;
		};
	}

	template <class T, class Engine, class Distributor>
	class Random final : public Detail::RandomBase
	{
	public:
		using Seed = typename Engine::result_type;
		using Parameter = typename Distributor::param_type;

	public:
		Random() : engine(randomDevice) {}

		T operator()(const Parameter& param) noexcept
		{
			Distributor distributor{ engine };
			return Distributor(param);
		}

		void SetSeed(Seed seed)
		{
			engine.seed(seed);
		}

	private:
		Engine engine;
	};
	
	using UniformIntRandom = Random<int, std::mt19937, std::uniform_int_distribution<int>>;
	using UniformFloatRandom = Random<float, std::mt19937, std::uniform_real_distribution<float>>;
	using NormalFloatRandom = Random<float, std::mt19937, std::normal_distribution<float>>;
	
}