#ifndef __PERCEPTRON_H__
#define __PERCEPTRON_H__
#include <inttypes.h>   /* for uint32_t */
#include <cstdio>

#define GH_LEN 128

#define LINES_S 1024
#define DEPTH_S 20

#define LINES_M 1024
#define DEPTH_M (GH_LEN - DEPTH_S)

#define THRESHOLD 107

#define ABS(X) ((X)>0?(X):(-X))

// ============================================================================
class perceptron_t {
	private:
        int weight_ntkn[LINES_S][DEPTH_S];  //separated weights not taken
        int weight_tkn[LINES_S][DEPTH_S];  //separated weights taken

        int weight_mixed[LINES_M][DEPTH_M];  //mixed weights
        
        bool global_history[GH_LEN];        //global branch outcomes
        uint64_t history_address[GH_LEN];   //last pc address

        int sum;                            //prediction sum

        // ====================================================================
		/// Methods
		// ====================================================================
        void shiftRegister_bool(bool* reg,bool value);
        void shiftRegister_uint64(uint64_t* reg,uint64_t value);
        uint32_t hash(uint64_t pc,uint64_t old_pc,uint32_t max_hash_size);
        
    public:
        uint64_t wrong_prediction;
        uint64_t correct_prediction;

        
		// ====================================================================
		/// Methods
		// ====================================================================
		void allocate();
        void update(bool prediction,bool outcome,uint64_t pc);
        bool predict(uint64_t pc);
};

#endif