#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>
#include<numeric>


class PayOff{
		public:
				PayOff() {};
				virtual ~PayOff() {};
				virtual double operator() (const double& S) const = 0;
};

class PayOffCall: public PayOff {
	private:
		double K;

	public:
		PayOffCall(const double& K_) { K = K_;}
		virtual ~PayOffCall() {};
		virtual double operator() (const double& S) const;
};

class PayOffPut: public PayOff {
	private:
		double K;
	public:
		PayOffPut(const double& K_);
		virtual ~PayOffPut() {};
		virtual double operator() (const double& S) const;
};




//---------
//PayOffCall
//---------

//PayOffCall::PayOffCall(const double& _K) { K = _K; }

double PayOffCall::operator() (const double& S) const{
	return std::max(S-K,0.0);
}

//---------
//PayOffPut
//---------

PayOffPut::PayOffPut(const double& _K) {
	K = _K;
}

double PayOffPut::operator() (const double& S) const {
	return std::max(K-S,0.0);
}


// Asian Options


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



// Base class construction
AsianOption::AsianOption(PayOff* _pay_off)
{
	pay_off = _pay_off;
}

// Arithmetic
AsianOptionArithmetic::AsianOptionArithmetic(PayOff* _pay_off):AsianOption(_pay_off) {}
//{
//	pay_off = _pay_off;
//}

double AsianOptionArithmetic::pay_off_price(const std::vector<double>& spot_prices) const
{
	unsigned num_times = spot_prices.size();
	double sum = std::accumulate(spot_prices.begin(), spot_prices.end(),0);
	double arith_mean = sum / static_cast<double>(num_times);
	return (*pay_off)(arith_mean);
}

// Geometric
AsianOptionGeometric::AsianOptionGeometric(PayOff* _pay_off): AsianOption(_pay_off) {}
//{
//	pay_off = _pay_off;
//}

double AsianOptionGeometric::pay_off_price(const std::vector<double>& spot_prices) const
{
	unsigned num_times = spot_prices.size();
	double log_sum = 0.0;
	for (int i=0; i<spot_prices.size();i++)
	{
		log_sum += log(spot_prices[i]);
	}
	double geom_mean = exp(log_sum/static_cast<double>(num_times) );
	return (*pay_off)(geom_mean);
}



double gaussian_box_muller() {
	double x = 0.0;
	double y = 0.0;
	double euclid_sq = 0.0;

	do{
		x = 2.0 * rand() / static_cast<double>(RAND_MAX) - 1;
		y = 2.0 * rand() / static_cast<double>(RAND_MAX) - 1;
		euclid_sq = x*x + y*y;
	}while (euclid_sq >= 1.0);

	return x*sqrt(-2*log(euclid_sq)/euclid_sq);
}


void calc_path_spot_prices(std::vector<double>& spot_prices,
		           const double& r,
			   const double& v,
			   const double& T) {
	double dt = T/static_cast<double>(spot_prices.size());
	double drift = exp(dt*(r - 0.5*v*v));
	double vol = sqrt(v*v*dt);

	for (int i=1; i<spot_prices.size(); i++)
	{
		double gauss_bm = gaussian_box_muller();
		spot_prices[i] = spot_prices[i-1] * drift * exp(vol*gauss_bm);
	}
}


int main(int argc, char **argv)
{
	unsigned num_sims = 100000;
	unsigned num_intervals = 250;
	double S = 30.0;
	double K = 29.0;
	double r = 0.08;
	double v = 0.3;
	double T = 1.00;
        std::vector<double> spot_prices(num_intervals, S);

	// create PayOff objects
	PayOff* pay_off_call = new PayOffCall(K);

	// create AsianOption objects
	AsianOptionArithmetic asian(pay_off_call);

	// calculate option prices
	double payoff_sum = 0.0;
	for (int i=0; i<num_sims; i++ )
	{
		calc_path_spot_prices(spot_prices, r, v, T);
		payoff_sum += asian.pay_off_price(spot_prices);
	}
	double discount_payoff_avg = (payoff_sum / static_cast<double>(num_sims)) * exp(-r*T);

	delete pay_off_call;

	// Finally we output the results
	std::cout<<"Number of Paths: "<<num_sims<<std::endl;
	std::cout<<"Number of Ints:  "<<num_intervals<<std::endl;
	std::cout<<"Underlying:      "<<S<<std::endl;
	std::cout<<"Strike:          "<<K<<std::endl;
	std::cout<<"Risk-Free Rate:  "<<r<<std::endl;
	std::cout<<"Volatility:      "<<v<<std::endl;
	std::cout<<"Maturity:        "<<T<<std::endl;

	std::cout<<"Asian Price:     "<<discount_payoff_avg <<std::endl;

	return 0;
}
