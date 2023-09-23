#ifndef __PERCEPTRON_H__
#define __PERCEPTRON_H__
#include <inttypes.h>   /* for uint32_t */
#include <cstdio>


#define CONST_N 2048
#define CONST_H 37
#define THETA (int)(1.93 * CONST_H + 14)
#define ABS(X) ((X)>0?(X):(-X))

// ============================================================================
class perceptron_t {
	private:
        int weight[CONST_N][CONST_H+1];
        
        int speculative_prediction_register[CONST_H+1];
        int outcome_prediction_register[CONST_H+1];

        bool speculative_global_history[CONST_H+1];
        bool old_SG[CONST_H+1];
        bool outcome_global_history[CONST_H+1];

        int index_history[CONST_H+1];
        int speculative_index_history[CONST_H+1];
        int old_speculative_index_history[CONST_H+1];

        int index;
        int y_out;  //Perceptron output

        // ====================================================================
		/// Methods
		// ====================================================================
        void shiftRegister_bool(bool* reg,bool value);
        void shiftRegister_int(int* reg,int value);
        void copy_bool(bool* target,bool* source);
        void copy_int(int* target,int* source);

    public:
        uint64_t wrong_prediction;
        uint64_t correct_prediction;

        
		// ====================================================================
		/// Methods
		// ====================================================================
		void allocate();
        void update(bool prediction,bool outcome);
        bool predict(uint64_t pc);
};

#endif