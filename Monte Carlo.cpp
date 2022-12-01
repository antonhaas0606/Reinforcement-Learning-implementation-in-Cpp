//
//  main.cpp
//  BJMC
//
//  Created by Anton Haas on 2020. 06. 23..
//  Copyright © 2020. Anton Haas. All rights reserved.
//

//find/enter state function: hit until in any state
//step function input: state and action output: sample successor state and reward(no discounting)
//Monte Carlo first visit



#include <iostream>

std::string dealervalues [11] = {" ","A","2","3","4","5","6","7","8","9","10"};
std::string playervalues [10] = {"21","20","19","18","17","16","15","14","13","12"};

long wins = 0;
long draws = 0;
long losses = 0;

long episodes = 40000000;   //how many games the program should play

bool dealerace;             //does the dealer have an ace
bool ace;                   //does the player have an ace

int playersum;              //summed value of all the cards that the player has
int dealersum;              //summed value of all the cards that the dealer has
int dealtcard;              //value of the dealt card

bool terminate;             //if this is true the eposide has to end

int reward;

int nextstate;
bool nextaction;
bool e;

struct states               //define a state
{
  int playersum;
  int dealersum;
    bool ace;
};

struct states state [200];                //200 possible states all stored in one matrix

bool policy [200];                        //if true in the corresponding state the player hits

float actionvaluefunction [200][2];       //predicted value of hitting or sticking in the corresponding state (in this order)
bool stateactionpair [200][2];            //keeping track of the state action pairs encountered during one episode
long stateactionpairvisits [200][2];      //keeping track of number of times each  state action pair was encountered throughout all episodes

void epsylon()       //probability of exploration (in this case: 0.1)
{
    int possibility = rand() % 100 + 1;
    if(possibility == 1)
        e = true;
    else
        e = false;
}

void fillstates ()        //setting up each individual state in the state matrix
{
    int n = 1;
    for (int i = 0; i < 100; i++)
    {
      state[i].playersum = n * 10 - i + 11;
      if(n != 1)
        state[i].dealersum = n;
      else
        state[i].dealersum = 11;

      if (i == n * 10 - 1)
        n++;
        
      state[i].ace = false;
    }
    n = 1;
    for (int i = 0; i < 100; i++)
    {
      state[i + 100].playersum = n * 10 - i + 11;
      if(n != 1)
        state[i + 100].dealersum = n;
      else
        state[i + 100].dealersum = 11;

      if (i == n * 10 - 1)
        n++;
        
      state[i].ace = true;
    }
}

int randomcard()        //deals a random card from the deck
{
    int dealcard = rand() % 13 + 1;
    if (dealcard < 11 && dealcard != 1)
        return dealcard;
    else if(dealcard != 1)
        return 10;
    else
        return 11;
}

void findfirststate ()    //each game starts with the dealer dealing two cards for himself and dealing cards to the player until it is impossible for him to go bust
{
    dealerace = false;
    
    dealersum += randomcard();    //concerning dealer (only one since we dont know the second one which is faced down)
    
    while(playersum < 12)         //concerning player
    {
        dealtcard = randomcard();
        
        if(dealtcard == 11 && ace)
            dealtcard = 1;
        
        if (dealtcard == 11)
            ace = true;
        
        playersum += dealtcard;
    }
    
    for (int i = 0; i < 200; i++) //finding state
    {
        if (state[i].dealersum == dealersum && state[i].playersum == playersum && state[i].ace == ace) //then we are in state i
      {
          nextstate = i;
      }
    }
    
}

void step (int k, bool action)             //one turn of the player who already knows the action he is taking from the e-greedy policy
{
    ace = state[k].ace;
    playersum = state[k].playersum;
    dealersum = state[k].dealersum;
  if(action)                             //player chooses to hit
  {
      if(stateactionpair[k][0] == false)
          stateactionpairvisits [k][0]++;
      
      stateactionpair[k][0] = true;
      dealtcard = randomcard();
      
      if(dealtcard == 11 && ace)
          dealtcard = 1;
      if(dealtcard == 11)
          ace = true;
      
      if (21 < playersum + dealtcard && !ace) //if player goes bust
      {
          terminate = true;
          reward = -1;
          losses++;
          //std::cout << reward <<std::endl;
      }
      else                                    //the game goes on
      {
          if(21 < playersum + dealtcard)
          {
              playersum -= 10;
              ace = false;
          }
          
          for (int i = 0; i < 200; i++) //finding the new state
          {
              if (state[i].dealersum == dealersum && state[i].playersum == playersum + dealtcard && state[i].ace == ace) //then we are in state i
            {
                nextstate = i;
                reward = 0;
            }
          }
      }
      
  }
  else                                 //player chooses to stick
  {
      
      if(stateactionpair[k][1] == false)
          stateactionpairvisits [k][1]++;
      
      stateactionpair[k][1] = true;
      terminate = true;
      
      if(dealersum == 11)
          dealerace = true;
      
      while (dealersum < 17)       //dealer hits (following his own policy)
      {
          dealtcard = randomcard();
          
          if(dealtcard == 11 && dealerace)
              dealtcard = 1;
          
          if(dealtcard == 11)
              dealerace = true;
          
          dealersum += dealtcard; //sum after new card is dealt
          
          if(dealersum > 21 && dealerace)
          {
              dealersum -= 10;
              dealerace = false;
          }
      }

      
      
      if (21 < dealersum || dealersum < playersum)          //evaluating the end result and recieving reward accordingly
      {
        reward = 1; //player wins
          wins++;
      }
      else if (playersum < dealersum)
      {
        reward = -1; //player loses
          losses++;
          
      }
      else
      {
        reward = 0; //they draw
          draws++;
      }

  }
}

void reset()                      //required reset after each episode(game played)
{
    for (int i = 0; i < 200; i++) //finding state
    {
        stateactionpair[i][0] = false;
        stateactionpair[i][1] = false;
    }
    reward = 0;
    dealersum = 0;
    playersum = 0;
    ace = false;
    terminate = false;
    findfirststate ();
}

void afterepisode()             //updating the value functions according to the Monte-Carlo learning method
{
    for (int i = 0; i < 200; i++)
    {
        if(stateactionpair[i][0] == true)
        {
            actionvaluefunction[i][0] += (1./(float)stateactionpairvisits[i][0])*((float)reward - actionvaluefunction[i][0]);
        }
        if(stateactionpair[i][1] == true)
        {
            actionvaluefunction[i][1] += (1./(float)stateactionpairvisits[i][1])*((float)reward - actionvaluefunction[i][1]);
        }
    }
}

void bestpolicy()        //judging from the action value function for each state the action with a higher value is determined to be the optimal action
{
    for (int i = 0; i < 200; i++)
    {
        if(actionvaluefunction[i][0] < actionvaluefunction[i][1])
            policy[i] = false; //stick
        else
            policy[i] = true; //hit
    }
}

int main(int argc, const char * argv[]) {                //main code which brings all the predefined functions together
    
    fillstates();
    
    for (int j = 0; j < episodes; j++)                   //for the first 3/4-s of the episodes it learns from them and after it follows the obtained optimal policy
    {
        if (j == episodes*3/4)
        {
            bestpolicy();
            wins = 0;
            draws = 0;
            losses = 0;
        }
            
        reset();
        while(! terminate)
        {
            epsylon();
            
            if(e) //random action
            {
                int coinflip = rand() % 2 + 1;
                if(coinflip == 1)
                    nextaction = false;
                else
                    nextaction = true;
            }
            else //greedy
            {
                if(actionvaluefunction[nextstate][0] < actionvaluefunction[nextstate][1])
                    nextaction = false; //stick
                else
                    nextaction = true; //hit
            }
            
            if (j > episodes*3/4)
            {
                nextaction = policy[nextstate];
            }
            
            step (nextstate, nextaction);
        }
        afterepisode();
    }
    
            //displaying the results in the form of two tables: one shows the action the player should take if he has no ace and the other shows the action he should take when having an ace
    
    std::cout << "draws: " << draws <<std::endl;
    std::cout << "wins: " << wins <<std::endl;
    std::cout << "losses: " << losses <<std::endl;
    std::cout << "total games played with best strategy: " << losses + draws + wins <<std::endl;
    std::cout << "" <<std::endl;
    
    bool n;
    
    for (int g = 0; g < 2; g++)
    {
        if (g == 1)
            std::cout << "soft hand" <<std::endl;
        else
            std::cout << "hard hand" <<std::endl;
        
        std::cout << "" <<std::endl;
        
        for (int i = 0; i < 11; i++)
        {
            if(i == 10)
                std::cout << dealervalues[i] <<std::endl;
            else
                std::cout << dealervalues[i] << "  " ;
        }
        
        for (int j = 0; j < 10; j++)
        {
            std::cout << playervalues[j];
            for (int i = 0; i < 10; i++)
            {
                if (g == 1)
                    n = policy[j+(i*10)];
                else
                    n = policy[100 + j+(i*10)];
                
                if(n)
                    std::cout << " H ";
                else
                    std::cout << " S ";
            }
            std::cout << "" << std::endl;
        }
        
        std::cout << "" << std::endl;
    }
    
    return 0;
}
