//
//  main.cpp
//  BJSARSAL
//
//  Created by Anton Haas on 2020. 06. 24..
//  Copyright © 2020. Anton Haas. All rights reserved.
//

#include <iostream>

std::string dealervalues [11] = {" ","A","2","3","4","5","6","7","8","9","10"};
std::string playervalues [10] = {"21","20","19","18","17","16","15","14","13","12"};

long episodes = 50000000;        //how many games the program should play

int playersum;                  //summed value of all the cards that the player has
int dealersum;                  //summed value of all the cards that the dealer has
int dealtcard;                  //value of the dealt card
bool ace;
bool dealerace;

bool terminate;                //if this is true the eposide has to end

int reward;
float discountvalue = 0.8;
float L = 0.8;
float tderror;
//float stepsize;

int nextstate;
bool nextaction;
bool e;

bool policy [200];                      //will determine the best action to take in each state

struct states
{
  int playersum;
  int dealersum;
  bool ace;
};

struct states state [200];                //200 possible states all stored in one matrix

float actionvaluefunction [200][2]; //hit or stick (in this order)
long stateactionpaivisits [200][2];
int eligability[200][2];

void epsylon() //probability of eploration (in this case: 0.1)
{
    int possibility = rand() % 10 + 1;
    if(possibility == 1)
        e = true;
    else
        e = false;
}


void egreedy()
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

void findfirststate ()     //each game starts with the dealer dealing two cards for himself and dealing cards to the player until it is impossible for him to go bust
{
    dealersum += randomcard();    //concerning dealer
    
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
    
    //std::cout << dealersum <<  " " << playersum <<std::endl;
}

void step (int k, bool action)              //one turn of the player who already knows the action he is taking
{
    ace = state[k].ace;
    playersum = state[k].playersum;
    dealersum = state[k].dealersum;
  if(action) //hit
  {
      stateactionpaivisits[k][0]++;
      eligability[k][0]++;
      
      dealtcard = randomcard();
      
      if(dealtcard == 11 && ace)
          dealtcard = 1;
      
      if(dealtcard == 11)
          ace = true;
      
      if (21 < playersum + dealtcard && !ace) //we go bust
      {
          terminate = true;
          reward = -1;
          actionvaluefunction[k][0] += (1./(float)stateactionpaivisits[k][0])*((float)reward - actionvaluefunction[k][0]);
          //std::cout << reward <<std::endl;
      }
      else
      {
          if(21 < playersum + dealtcard) //using an ace
          {
              ace = false;
              playersum -= 10;
          }
          
          for (int i = 0; i < 200; i++) //finding state
          {
              if (state[i].dealersum == dealersum && state[i].playersum == playersum + dealtcard && state[i].ace == ace) //then we are in state i
            {
                nextstate = i;
                reward = 0;
            }
          }
          
          egreedy();
          
          if(nextaction)
              tderror = (float)reward + discountvalue * actionvaluefunction[nextstate][0] - actionvaluefunction[k][0];
          else
              tderror = (float)reward + discountvalue * actionvaluefunction[nextstate][1] - actionvaluefunction[k][0];
          
          for (int i = 0; i < 200; i++)
          {
              actionvaluefunction[i][0] +=(1./(float)stateactionpaivisits[k][0]) * tderror * eligability[i][0];
              eligability[i][0] *= L * discountvalue;
              eligability[i][1] *= L * discountvalue;
          }
      }
          
      //std::cout << state[nextstate].dealersum <<  " " << state[nextstate].playersum <<std::endl;
      
  }
  else //stick
  {
      stateactionpaivisits[k][1]++;
      eligability[k][1]++;
      //std::cout << "stick" <<std::endl;
      terminate = true;
      
      if(dealersum == 11)
          dealerace = true;
      
      while (dealersum < 17) //dealer hits (following his own policy)
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
          reward = 1; //we win
      }
      else if (playersum < dealersum)
      {
          reward = -1; //we lose
      }
      else
      {
          reward = 0; //we draw
      }
      
      actionvaluefunction[k][1] += (1./(float)stateactionpaivisits[k][1])*((float)reward - actionvaluefunction[k][1]);
      
      
      //std::cout << dealersum <<  " " << playersum <<std::endl;
      //std::cout << reward <<std::endl;

  }
}

void reset()                           //required reset after each episode(game played)
{
    for (int i = 0; i < 200; i++)
    {
        eligability[i][0] = 0;
        eligability[i][1] = 0;
    }
    reward = 0;
    dealersum = 0;
    playersum = 0;
    ace = false;
    dealerace = false;
    terminate = false;
    findfirststate ();
    egreedy();
}


void bestpolicy()        //judging from the action value function for each state the action with a higher value is determined to be the optimal action
{
    for (int i = 0; i < 200; i++) //finding state
    {
        if(actionvaluefunction[i][0] < actionvaluefunction[i][1])
            policy[i] = false; //stick
        else
            policy[i] = true; //hit
    }
}

int main(int argc, const char * argv[]) {     //main code which brings all the predefined functions together
    
    fillstates();
    
    for (int j = 0; j < episodes; j++) //finding state
    {
        reset();
        while(! terminate)
        {
            step (nextstate, nextaction);
        }
    }
    
    bestpolicy();
    
    //displaying the results in the form of two tables: one shows the action the player should take if he has no ace and the other shows the action he should take when having an ace
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
