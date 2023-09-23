#ifndef __PROCESSOR__
#define __PROCESSOR__


#include "btb.hpp"
#include "branch_predictor.hpp"

// ============================================================================
// ============================================================================
class processor_t {
    private:    

		int _latency;
		uint64_t latecy_cycles;
		bool checkBranchPrediction;
		bool checkConditionalMiss;
		bool prediction;
		bool updateBTB;
		uint64_t nextOpcodeAddress;
		uint64_t previousOpcodeAddress;
		btb_t _btb;
		perceptron_t _perceptron;
    
    public:

		// ====================================================================
		/// Methods
		// ====================================================================
		processor_t();
	    void allocate();
	    void clock();
	    void statistics();
};


#endif