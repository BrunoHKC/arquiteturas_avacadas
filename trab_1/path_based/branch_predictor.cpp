#include "branch_predictor.hpp"

// =====================================================================
void perceptron::shiftRegister(bool* reg)
{
    for(int i = 1; i < CONST_H;i++)
    {
        reg[i] = reg[i-1];
    }
}
// =====================================================================
void perceptron::copy(bool* target,bool* source,int size)
{
    for(int i = 0; i < size;i++)
    {
        target[i] = source[i];
    }
}



// =====================================================================
void perceptron::allocate()
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
        speculative_prediction_register[j];
        outcome_prediction_register[j];

        speculative_global_history[j];
        outcome_global_history[j];
    }
    speculative_prediction_register[CONST_H];
    outcome_prediction_register[CONST_H];
}

// =====================================================================
void perceptron::update(bool prediction,bool outcome)
{
    if(prediction != outcome || ABS(y_out) <= THETA)
    {
        int val = outcome?1:-1;
        weight[i][0] = outcome?weight[i][0]+1:weight[i][0]-1;
        for(int j = 1;j <= CONST_H;j++)
        {
            int k = index_history[j];
            weight[k][j] = (outcome==old_SR[j])?weight[k][j]+1:weight[k][j]-1;
        }
    }
    shiftRegister(outcome_global_history);
    outcome_global_history[0] = outcome;
    if(prediction != outcome)
    {
        copy(speculative_global_history,outcome_global_history,CONST_H);
        copy(speculative_prediction_register,outcome_prediction_register,CONST_H+1);
    }
}

// =====================================================================
bool perceptron::predict(uint64_t pc)
{
    bool prediction;
    index = pc % CONST_N;
    y_out = speculative_prediction_register[CONST_H] + weight[i,0];

    prediction = y >= 0;

    copy(old_SR,speculative_prediction_register,CONST_H+1);
    shiftRegister(index_history);
    index_history[0] = index;

    bool temp_SR[CONST_H+1];
    for(int j = i; j < CONST_H; j++)
    {
        int kj = CONST_H - j;
        if(prediction)
        {
            temp_SR[kj+1] = speculative_prediction_register[kj] + weight[i][j];
        }
        else
        {
            temp_SR[kj+1] = speculative_prediction_register[kj] - weight[i][j];
        }
    }

    copy(speculative_prediction_register,temp_SR,CONST_H+1);
    speculative_prediction_register[0] = 0;
    shiftRegister(speculative_global_history);
    speculative_global_history[0] = prediction;

    return prediction;
}