#ifndef __BTB_H__
#define __BTB_H__
#include <inttypes.h>   /* for uint32_t */
#include <cstdio>


#define CONST_N 1024
#define CONST_H 7
#define THETA (int)(1.93 CONST_H + 14)
#define ABS(X) ((X)>0?(X):(-X))

// ============================================================================
class perceptron {
	private:
        int weights[CONST_N][CONST_H + 1];
        
        int speculative_prediction_register[CONST_H + 1];
        int outcome_prediction_register[CONST_H + 1];

        bool speculative_global_history[CONST_H];
        bool outcome_global_history[CONST_H];

        int index_history[CONST_H];
        int old_SR[CONST_H+1];

        int index;
        int y_out;  //Perceptron output

        // ====================================================================
		/// Methods
		// ====================================================================
        void shiftRegister(bool* reg);
        void copy(bool* target,bool* source,int size)

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