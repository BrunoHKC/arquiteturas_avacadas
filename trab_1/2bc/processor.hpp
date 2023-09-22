#ifndef __PROCESSOR__
#define __PROCESSOR__


#include "btb.hpp"

// ============================================================================
// ============================================================================
class processor_t {
    private:    

		int _latency;
		bool checkBranchPrediction;
		bool checkConditionalMiss;
		bool prediction;
		bool updateBTB;
		uint64_t nextOpcodeAddress;
		uint64_t previousOpcodeAddress;
		btb_t _btb;
    
    
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