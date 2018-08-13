#include <vector>

class Twiddle {
public:
	Twiddle();

	~Twiddle();

	void init(std::vector<double>& params);

	void tuneParams(double error);

	void getParams(std::vector<double>& params);

	double getSumdParams();

private:
	bool first_time;
	int phase_;
	int p_id_;
	double error_min_;
	std::vector<double> params_;
	std::vector<double> d_params_;
};