#include "branch_predictor.hpp"


/*
int satured(int value,bool increment)
{
    if(increment)
    {
        if(value < 2147483646)
            value++;
    }
    else
    {
        if(value > -2147483647)
            value--;
    }
    return value;
}
*/

// =====================================================================
void perceptron_t::shiftRegister_bool(bool* reg)
{
    for(int i = 0; i < (CONST_H+1);i++)
    {
        reg[i] = reg[i-1];
    }
}
// =====================================================================
void perceptron_t::shiftRegister_int(int* reg)
{
    for(int i = 0; i < (CONST_H+1);i++)
    {
        reg[i] = reg[i-1];
    }
}
// =====================================================================
void perceptron_t::copy_bool(bool* target,bool* source)
{
    for(int i = 0; i < (CONST_H+1);i++)
    {
        target[i] = source[i];
    }
}
// =====================================================================
void perceptron_t::copy_int(int* target,int* source)
{
    for(int i = 0; i < (CONST_H+1);i++)
    {
        target[i] = source[i];
    }
}



// =====================================================================
void perceptron_t::allocate()
{
    //Reset weight matrix
    for(int i = 0; i < CONST_N;i++)
    {
        for(int j = 0; j < (CONST_H + 1);j++)
        {
            weight[i][j] = 0;
        }   
    }

    //Reset registers
    for(int j = 0; j < (CONST_H);j++)
    {
        speculative_prediction_register[j] = 0;
        outcome_prediction_register[j] = 0;

        speculative_global_history[j] = 0;
        outcome_global_history[j] = 0;
    }
    speculative_prediction_register[CONST_H] = 0;
    outcome_prediction_register[CONST_H] = 0;
}

// =====================================================================
bool perceptron_t::predict(uint64_t pc)
{
    bool prediction;
    // hash the pc and save the index history
    index = pc % CONST_N;
    shiftRegister_int(index_history);
    index_history[0] = index;

    // save the old value of SG
    copy_bool(old_SG,speculative_global_history);
    
    // predict
    y_out = speculative_prediction_register[CONST_H] + weight[index][0];
    prediction = y_out >= 0;

    // speculate next prediction
    for(int j = 1; j <= CONST_H; j++)
    {
        int k = CONST_H - j;
        speculative_prediction_register[k+1] = speculative_prediction_register[k];
        speculative_prediction_register[k+1] += prediction?weight[index][j]:-weight[index][j];
    }

    speculative_prediction_register[0] = 0;

    shiftRegister_bool(speculative_global_history);
    speculative_global_history[0] = prediction;

    return prediction;
}
// =====================================================================
void perceptron_t::update(bool prediction,bool outcome)
{
    //non speculative predictions
    for(int j = 1; j <= CONST_H; j++ )
    {
        int k = CONST_H-j;
        outcome_prediction_register[k+1] = outcome_prediction_register[k];
        outcome_prediction_register[k+1] += outcome?weight[index][j]:-weight[index][j];
    }
    outcome_prediction_register[0] = 0;

    shiftRegister_bool(outcome_global_history);
    outcome_global_history[0] = outcome;

    // handle missprediction
    if(prediction != outcome)
    {
        for(int j = 0;j <= CONST_H; j++)
        {
            speculative_prediction_register[j] = outcome_prediction_register[j];
            speculative_global_history[j] = outcome_global_history[j];
        }
    }

    // perceptron learning
    if(prediction != outcome || ABS(y_out) <= THETA)
    {
        weight[index][0] = outcome?weight[index][0]+1:weight[index][0]-1;
        //weight[index][0] = satured(weight[index][0],outcome);
        for(int j = 1;j <= CONST_H;j++)
        {
            int k = index_history[j];
            weight[k][j] = (outcome==old_SG[j])?weight[k][j]+1:weight[k][j]-1;
            //weight[k][j] = satured(weight[k][j],outcome==old_SG[j]);
        }
    }
    
    // update statistics
    if(prediction != outcome)
    {
        wrong_prediction++;
    }
    else{
        correct_prediction++;
    }
}