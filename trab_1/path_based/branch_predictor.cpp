#include "branch_predictor.hpp"


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
void perceptron_t::shiftRegister_bool(bool* reg,bool value)
{
    for(int i = 1; i <= CONST_H;i++)
    {
        reg[i] = reg[i-1];
    }
    reg[1] = value; //boolean vectors are addressed by [1..h] indexs
}
// =====================================================================
void perceptron_t::shiftRegister_int(int* reg,int value)
{
    for(int i = 1; i <= CONST_H;i++)
    {
        reg[i] = reg[i-1];
    }
    reg[0] = value;
}
// =====================================================================
void perceptron_t::copy_bool(bool* target,bool* source)
{
    for(int i = 0; i <= CONST_H;i++)
    {
        target[i] = source[i];
    }
}
// =====================================================================
void perceptron_t::copy_int(int* target,int* source)
{
    for(int i = 0; i <= CONST_H;i++)
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
        for(int j = 0; j <= CONST_H;j++)
            weight[i][j] = 0;
    }

    //Reset registers
    for(int j = 0; j <= CONST_H;j++)
    {
        speculative_prediction_register[j] = 0;
        outcome_prediction_register[j] = 0;

        speculative_global_history[j] = false;
        outcome_global_history[j] = false;

        index_history[j] = 0;
    }

    wrong_prediction = 0;
    correct_prediction = 0;
    
}

// =====================================================================
bool perceptron_t::predict(uint64_t pc)
{
    bool prediction;
    // hash the pc and predict using partial sum + bias
    index = pc % CONST_N;
    y_out = speculative_prediction_register[CONST_H] + weight[index][0];
    prediction = y_out >= 0;

    // insert the index in the speculative index vector
    shiftRegister_int(speculative_index_history,index);

    // save old versions of SG and Sv
    copy_int(old_speculative_index_history,speculative_index_history);
    copy_bool(old_SG,speculative_global_history);
    

    // speculate next prediction
    for(int j = 1; j <= CONST_H; j++)
    {
        int k = CONST_H - j;
        if(prediction)
            speculative_prediction_register[k+1] = speculative_prediction_register[k] + weight[index][j];
        else
            speculative_prediction_register[k+1] = speculative_prediction_register[k] - weight[index][j];
    }
    speculative_prediction_register[0] = 0;

    shiftRegister_bool(speculative_global_history,prediction);

    return prediction;
}
// =====================================================================
void perceptron_t::update(bool prediction,bool outcome)
{
    //non speculative values
    for(int j = 1; j <= CONST_H; j++ )
    {
        int k = CONST_H-j;
        if(outcome)
            outcome_prediction_register[k+1] = outcome_prediction_register[k] + weight[index][j];
        else
            outcome_prediction_register[k+1] = outcome_prediction_register[k] - weight[index][j];
    }
    outcome_prediction_register[0] = 0;

    shiftRegister_bool(outcome_global_history,outcome);
    shiftRegister_int(index_history,index);

    // perceptron learning
    if(prediction != outcome || ABS(y_out) <= THETA)
    {
        weight[index][0] = satured(weight[index][0],outcome);
        for(int j = 1;j <= CONST_H;j++)
        {
            int k = old_speculative_index_history[j-1];
            weight[k][j] = satured(weight[k][j],outcome==old_SG[j]);
        }
    }

    // handle missprediction
    if(prediction != outcome)
    {
        copy_int(speculative_prediction_register,outcome_prediction_register);
        copy_bool(speculative_global_history,outcome_global_history);
        copy_int(speculative_index_history,index_history);
    }

    // update statistics
    if(prediction != outcome)
        wrong_prediction++;
    else
        correct_prediction++;
    
}