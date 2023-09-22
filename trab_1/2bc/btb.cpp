#include "btb.hpp"

// =====================================================================
void btb_t::update2BC(uint64_t pc,int set,bool taken)
{
    if(taken)
    {
        _table[INDEX(pc)][set].twoBitCounter = MIN(_table[INDEX(pc)][set].twoBitCounter++,3);
    }
    else{
        _table[INDEX(pc)][set].twoBitCounter = MAX(_table[INDEX(pc)][set].twoBitCounter--,0);
    }
}

// =====================================================================
void btb_t::update(uint64_t pc,uint64_t cycle,bool taken)
{
    /// Check if table contains the passed pc
    int leastRecentUsed = ASSOCIATIVE_SETS;
    for(int set = 0; set < ASSOCIATIVE_SETS; set++)
    {
        if(_table[INDEX(pc)][set].valid && _table[INDEX(pc)][set].tag == TAG(pc)){
            /// Found the entry
            _table[INDEX(pc)][set].last_access_cycle = cycle;
            _table[INDEX(pc)][set].valid = true;
            _table[INDEX(pc)][set].tag = TAG(pc);
            update2BC(pc,set,taken);
            return;
        }

        
        if(!_table[INDEX(pc)][set].valid || leastRecentUsed == ASSOCIATIVE_SETS)
        {
            leastRecentUsed = set;
        }
        else{
            // compare current set with the other candidate
            if(_table[INDEX(pc)][leastRecentUsed].valid && _table[INDEX(pc)][leastRecentUsed].last_access_cycle > _table[INDEX(pc)][set].last_access_cycle)
            {
                leastRecentUsed = set;
            }
        }
    }

    /// remove the least recent used entry
    _table[INDEX(pc)][leastRecentUsed].last_access_cycle = cycle;
    _table[INDEX(pc)][leastRecentUsed].valid = true;
    _table[INDEX(pc)][leastRecentUsed].tag = TAG(pc);
    _table[INDEX(pc)][leastRecentUsed].twoBitCounter = 0;
}

// =====================================================================
void btb_t::allocate()
{
    btb_hit = 0;
    btb_miss = 0;
    wrongPrediction = 0;
    correctPrediction = 0;

    /// Initailize BTB entries
    for(int line = 0; line < SET_AMOUNT;line++)
    {
        for(int set = 0; set < ASSOCIATIVE_SETS;set++)
        {
            _table[line][set].valid = false;
            _table[line][set].twoBitCounter = 0;
            _table[line][set].last_access_cycle = 0;
        }    
    }
}
// =====================================================================
bool btb_t::hit(uint64_t  pc)
{
    for(int set = 0; set < ASSOCIATIVE_SETS; set++)
    {
        if(_table[INDEX(pc)][set].valid && _table[INDEX(pc)][set].tag == TAG(pc)){
            btb_hit++;
            return true;
        }
    }
    btb_miss++;
    return false;
}

bool btb_t::predict(uint64_t  pc)
{   
    for(int set = 0; set < ASSOCIATIVE_SETS; set++)
    {
        if(_table[INDEX(pc)][set].valid && _table[INDEX(pc)][set].tag == TAG(pc)){
            return _table[INDEX(pc)][set].twoBitCounter>>1;
        }
    }
    return false;
}


// =====================================================================
void btb_t::updatePredictionStatistics(bool misprediction)
{
    if(misprediction)
    {
        wrongPrediction++;
    }
    else{
        correctPrediction++;
    }
}
