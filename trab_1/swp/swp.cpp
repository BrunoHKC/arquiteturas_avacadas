#include "swp.hpp"


// avoid underflow/overflow
int satured(int value,bool increment)
{
    if(increment)
    {
        if(value < 255) //weights implemented with int_8
            value++;
    }
    else
    {
        if(value > -255) //weights implemented with int_8
            value--;
    }
    return value;
}
// =====================================================================
void perceptron_t::shiftRegister_uint64(uint64_t* reg,uint64_t value)
{
    for(int i = 1; i <= GH_LEN;i++)
    {
        reg[i] = reg[i-1];
    }
    reg[0] = value;
}
// =====================================================================
void perceptron_t::shiftRegister_bool(bool* reg,bool value)
{
    for(int i = 1; i <= GH_LEN;i++)
    {
        reg[i] = reg[i-1];
    }
    reg[0] = value;
}
// =====================================================================
void perceptron_t::allocate()
{
    //Reset separated weight matrix
    for(int i = 0; i < LINES_S;i++)
    {
        for(int j = 0; j < DEPTH_S;j++)
        {
            weight_ntkn[i][j] = 0;
            weight_tkn[i][j] = 0;
        }
    }

    //Reset mixed weight matrix
    for(int i = 0; i < LINES_M;i++)
    {
        for(int j = 0; j < DEPTH_M;j++)
        {
            weight_mixed[i][j] = 0;
        }
    }

    //Reset GH and HA
    for(int i = 0; i < GH_LEN; i++)
    {
        global_history[i] = 0;
        history_address[i] = 0;
    }

    wrong_prediction = 0;
    correct_prediction = 0;
    
}

// =====================================================================
uint32_t perceptron_t::hash(uint64_t pc,uint64_t old_pc,uint32_t max_hash_value)
{
    return (pc ^ old_pc)%max_hash_value;
}

// =====================================================================
bool perceptron_t::predict(uint64_t pc)
{
    sum = 0;

    //most DEPH_S branches using seprated tables
    for(int i = 0; i < DEPTH_S; i++)
    {
        int index = hash(pc,history_address[i],LINES_S);
        if(global_history[i])
            sum += weight_tkn[index][i];
        else
            sum += weight_ntkn[index][i];
    }

    //next DEPH_M branches using mixed tables
    for(int i = 0; i < DEPTH_M; i++)
    {
        int index = hash(pc,history_address[i],LINES_M);
        if(global_history[i + DEPTH_S])
            sum += weight_mixed[index][i];
        else
            sum += weight_mixed[index][i];
    }

    return sum >= 0;
}
// =====================================================================
void perceptron_t::update(bool prediction,bool outcome,uint64_t pc)
{
    if(ABS(sum) < THRESHOLD || prediction != outcome)
    {
        // update accessed separated tables tkn/ntkn
        for(int i = 0; i < DEPTH_S; i++)
        {
            int index = hash(pc,history_address[i],LINES_S);
            if(global_history[i])
                //weight taken used
                weight_tkn[index][i] = satured(weight_tkn[index][i],outcome);
            
            else
                //weight not taken used
                weight_ntkn[index][i] = satured(weight_ntkn[index][i],outcome);
        }

        //next DEPH_M branches using mixed tables
        for(int i = 0; i < DEPTH_M; i++)
        {
            int index = hash(pc,history_address[i],LINES_M);
            weight_mixed[index][i] = satured(weight_mixed[index][i],global_history[i + DEPTH_S]);
        }
    }

    //insert new address and outcome
    shiftRegister_bool(global_history,outcome);
    shiftRegister_uint64(history_address,pc);


    // update statistics
    if(prediction != outcome)
        wrong_prediction++;
    else
        correct_prediction++;
    
}