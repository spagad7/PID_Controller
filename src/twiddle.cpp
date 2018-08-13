#include "twiddle.h"
#include <limits>
#include <iostream>

Twiddle::Twiddle() {

}

Twiddle::~Twiddle() {

}

void Twiddle::init(std::vector<double>& params) {
	// init params and d_params
	for(uint32_t i = 0; i < params.size(); ++i) {
		params_.push_back(params[i]);
	}
	d_params_.push_back(0.01);
	d_params_.push_back(0.00001);
	d_params_.push_back(0.05);
	first_time = true;
	phase_ = 0;
	p_id_ = 0;
	error_min_ = std::numeric_limits<double>::max();
}

void Twiddle::tuneParams(double error) {
	if(first_time) {
		// update parameters
		params_[p_id_] += d_params_[p_id_];
		first_time = false;
		return;
	}

	if(phase_ == 0) {
		std::cout << "phase 0" << std::endl;
		// check error
		if(error < error_min_) {
			error_min_ = error;
			d_params_[p_id_] *= 1.1;
			// move to next parameter in next cycle
			p_id_ = (p_id_ + 1) % 3;
			// repeat this phase in next cycle
			phase_ = (phase_ + 2) % 2;
			// update parameters
			params_[p_id_] += d_params_[p_id_];
		}
		else {
			params_[p_id_] -= (2 * d_params_[p_id_]);
			// go to next phase in the next cycle
			phase_ = (phase_ + 1) % 2;
		}
	}
	else if(phase_ == 1) {
		std::cout << "phase 1" << std::endl;
		if(error < error_min_) {
			error_min_ = error;
			d_params_[p_id_] *= 1.1;
		}
		else {
			params_[p_id_] += d_params_[p_id_];
			d_params_[p_id_] *= 0.9;
		}
		// move to next parameter
		p_id_ = (p_id_ + 1) % 3;
		phase_ = (phase_ + 1) % 2;
		// update parameters
		params_[p_id_] += d_params_[p_id_];
	}
}

void Twiddle::getParams(std::vector<double>& params) {
	params = params_;
}

double Twiddle::getSumdParams() {
	double sumdParams = 0.0;
	for (uint32_t i = 0; i < d_params_.size(); ++i) {
		sumdParams += d_params_[i];
	}
	return sumdParams;
}