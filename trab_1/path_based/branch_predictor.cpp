#include "branch_predictor.hpp"

// =====================================================================
void perceptron_t::shiftRegister_bool(bool* reg,int size)
{
    for(int i = 1; i < size;i++)
    {
        reg[i] = reg[i-1];
    }
}
// =====================================================================
void perceptron_t::shiftRegister_int(int* reg,int size)
{
    for(int i = 1; i < size;i++)
    {
        reg[i] = reg[i-1];
    }
}
// =====================================================================
void perceptron_t::copy_bool(bool* target,bool* source,int size)
{
    for(int i = 0; i < size;i++)
    {
        target[i] = source[i];
    }
}
// =====================================================================
void perceptron_t::copy_int(int* target,int* source,int size)
{
    for(int i = 0; i < size;i++)
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
    index = pc % CONST_N;
    y_out = speculative_prediction_register[CONST_H] + weight[index][0];

    prediction = y_out >= 0;

    shiftRegister_int(index_history,CONST_H);
    index_history[0] = index;
    copy_int(old_SR,speculative_prediction_register,CONST_H+1);
    

    int temp_SR[CONST_H+1];
    for(int j = 1; j <= CONST_H; j++)
    {
        int kj = CONST_H - j;
        temp_SR[kj+1] = speculative_prediction_register[kj];
        temp_SR[kj+1] += prediction?weight[index][j]:-weight[index][j];
    }

    copy_int(speculative_prediction_register,temp_SR,CONST_H+1);
    speculative_prediction_register[0] = 0;
    shiftRegister_bool(speculative_global_history,CONST_H);
    speculative_global_history[0] = prediction;

    return prediction;
}
// =====================================================================
void perceptron_t::update(bool prediction,bool outcome)
{
    if(prediction != outcome || ABS(y_out) <= THETA)
    {
        weight[index][0] = outcome?weight[index][0]+1:weight[index][0]-1;
        for(int j = 1;j <= CONST_H;j++)
        {
            int k = index_history[j-1];
            weight[k][j] = (outcome==old_SR[j])?weight[k][j]+1:weight[k][j]-1;
        }
    }
    shiftRegister_bool(outcome_global_history,CONST_H);
    outcome_global_history[0] = outcome;
    if(prediction != outcome)
    {
        copy_bool(speculative_global_history,outcome_global_history,CONST_H);
        copy_int(speculative_prediction_register,outcome_prediction_register,CONST_H+1);
    }

    if(prediction != outcome)
    {
        wrong_prediction++;
    }
    else{
        correct_prediction++;
    }
}