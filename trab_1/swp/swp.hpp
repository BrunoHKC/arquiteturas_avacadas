#ifndef __PERCEPTRON_H__
#define __PERCEPTRON_H__
#include <inttypes.h>   /* for uint32_t */
#include <cstdio>

#define GH_LEN 65

#define LINES_S 1024
#define DEPTH_S 20

#define LINES_M1 1024
#define DEPTH_M1 16

#define LINES_M2 512
#define DEPTH_M2 (GH_LEN - DEPTH_S - DEPTH_M1)

#define THRESHOLD 107
#define TC_LOW -64
#define TC_HIGH 64


#define ABS(X) ((X)>0?(X):(-X))

// ============================================================================
class perceptron_t {
	private:
        int weight_ntkn[LINES_S][DEPTH_S];  //separated weights not taken
        int weight_tkn[LINES_S][DEPTH_S];  //separated weights taken

        int weight_mixed1[LINES_M1][DEPTH_M1];  //mixed weights1
        int weight_mixed2[LINES_M2][DEPTH_M2];  //mixed weights2
        
        bool global_history[GH_LEN];        //global branch outcomes
        uint64_t history_address[GH_LEN];   //last pc address

        //int threshold_counter;
        //int theta;

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