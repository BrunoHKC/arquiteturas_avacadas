#include "branch_predictor.hpp"



int satured(int value,bool increment)
{
    if(increment)
    {
        if(value < 255)
            value++;
    }
    else
    {
        if(value > -255)
            value--;
    }
    return value;
}

// =====================================================================
void perceptron_t::shiftRegister_bool(bool* reg,bool value)
{
    for(int i = 0; i <= CONST_H;i++)
    {
        reg[i] = reg[i-1];
    }
    reg[1] = value;
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
    // hash the pc and save the index history
    index = pc % CONST_N;
    shiftRegister_int(speculative_index_history,index);

    // save the old value of SG and v
    copy_int(old_speculative_v,speculative_index_history);
    copy_bool(old_SG,speculative_global_history);
    
    // predict
    y_out = speculative_prediction_register[CONST_H] + weight[index][0];
    prediction = y_out >= 0;

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
    //TODO: send pc
    //index = pc % CONST_N;

    //non speculative predictions
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

    // handle missprediction
    if(prediction != outcome)
    {
        copy_int(speculative_prediction_register,outcome_prediction_register);
        copy_bool(speculative_global_history,outcome_global_history);
        copy_int(speculative_index_history,index_history);
        /*for(int j = 0;j <= CONST_H; j++)
        {
            speculative_prediction_register[j] = outcome_prediction_register[j];
            speculative_global_history[j] = outcome_global_history[j];
            speculative_index_history[j] = index_history[j];
        }
        */
    }

    // perceptron learning
    if(prediction != outcome || ABS(y_out) <= THETA)
    {
        /*
        if(outcome)
            weight[index][0] = weight[index][0]+1;
        else
            weight[index][0] = weight[index][0]-1;
        */
        weight[index][0] = satured(weight[index][0],outcome);
        for(int j = 1;j <= CONST_H;j++)
        {
            int k = old_speculative_v[j];
            //weight[k][j] = (outcome==old_SG[j])?weight[k][j]+1:weight[k][j]-1;
            weight[k][j] = satured(weight[k][j],outcome==old_SG[j]);
        }
    }
    // update statistics
    if(prediction != outcome)
        wrong_prediction++;
    else
        correct_prediction++;
    
}