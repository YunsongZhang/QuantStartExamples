#ifndef _ASIAN_H
#define _ASIAN_H

#include<vector>
#include "payoff.hpp"

class AsianOption {
	protected:
		PayOff* pay_off;

	public:
		AsianOption(PayOff* _pay_off);
		virtual ~AsianOption() {};

		virtual double pay_off_price(const std::vector<double>& spot_prices) const = 0;
};

class AsianOptionArithmetic: public AsianOption{
	public:
		AsianOptionArithmetic(PayOff* _pay_off);
		virtual ~AsianOptionArithmetic() {};

		virtual double pay_off_price(const std::vector<double>& spot_prices) const;
};

class AsianOptionGeometric: public AsianOption{
	public:
		AsianOptionGeometric(PayOff* _pay_off);
		virtual ~AsianOptionGeometric() {};

		virtual double pay_off_price(const std::vector<double>& spot_prices) const;
};


#endif
