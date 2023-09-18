#ifndef __BTB_H__
#define __BTB_H__
#include <inttypes.h>   /* for uint32_t */
#include <cstdio>


#define SET_AMOUNT 1024
#define ASSOCIATIVE_SETS 12
#define TAG(A) ((A)>>10)
#define INDEX(A) ((A)&1023)
#define MAX(A,B) ((A)>(B)?(A):(B))
#define MIN(A,B) ((A)<(B)?(A):(B))

typedef struct btb_entry_t {
    uint32_t tag;
    bool valid;
    int twoBitCounter;
    uint64_t last_access_cycle;
} btb_entry_t;


// ============================================================================
class btb_t {
	private:
        btb_entry_t _table[SET_AMOUNT][ASSOCIATIVE_SETS]; // Corrected data type
        
    public:
        uint64_t btb_hit;
        uint64_t btb_miss;

        uint64_t wrongPrediction;
        uint64_t correctPrediction;
		// ====================================================================
		/// Methods
		// ====================================================================
		void allocate();
        void updatePredictionStatistics(bool misprediction);
        bool hit(uint64_t pc);
        bool predict(uint64_t pc);
        void update(uint64_t pc,uint64_t cycle,bool taken);
        void update2BC(uint64_t pc,int set,bool taken);
};

#endif